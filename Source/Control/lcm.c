// Least common multiple of the integer parts of Pd floats.

#include <m_pd.h>
#include <float.h>
#include <math.h>
#include <stdint.h>

#if PD_FLOATSIZE == 64
#define LCM_PREC DBL_MANT_DIG
#define LCM_MAX_EXP DBL_MAX_EXP
#define LIMB_BITS 11
#else
#define LCM_PREC FLT_MANT_DIG
#define LCM_MAX_EXP FLT_MAX_EXP
#define LIMB_BITS 32
#endif
#define LIMB_COUNT ((LCM_MAX_EXP + LIMB_BITS - 1) / LIMB_BITS)
#define LIMB_MASK ((UINT64_C(1) << LIMB_BITS) - 1)

/* Store the odd part and the power of two separately. Every input's odd
   part fits in its significand. A limb times that significand fits in 64 bits. */
typedef struct{
    uint32_t limb[LIMB_COUNT];
    int used, twos;
}t_lcmint;

static const t_lcmint lcm_one = {{1}, 1, 0};
static t_class *lcm_class;

typedef struct{
    t_object x_obj;
    t_float x_left;
    t_lcmint x_right;
}t_lcm;

static void lcm_include(t_lcmint *n, t_float f){
    double value = trunc(fabs(f));
    if(!n->used || value <= 1)
        return;
    int exp;
    double fraction = frexp(value, &exp);
    uint64_t odd = (uint64_t)ldexp(fraction, LCM_PREC);
    int twos = exp - LCM_PREC;
    while(!(odd & 1)){
        odd >>= 1;
        twos++;
    }
    if(twos > n->twos)
        n->twos = twos;

    /* gcd(n, odd) = gcd(odd, n % odd). */
    uint64_t remainder = 0;
    for(int i = n->used; i--;)
        remainder = ((remainder << LIMB_BITS) | n->limb[i]) % odd;
    uint64_t gcd = odd;
    while(remainder){
        uint64_t next = gcd % remainder;
        gcd = remainder;
        remainder = next;
    }
    uint64_t factor = odd / gcd, carry = 0;
    if(factor == 1)
        return;
    for(int i = 0; i < n->used; i++){
        uint64_t product = n->limb[i] * factor + carry;
        n->limb[i] = product & LIMB_MASK;
        carry = product >> LIMB_BITS;
    }
    while(carry){
        if(n->used == LIMB_COUNT){
            n->used = 0;  // exceeds the Pd float range
            return;
        }
        n->limb[n->used++] = carry & LIMB_MASK;
        carry >>= LIMB_BITS;
    }
}

static t_float lcm_result(const t_lcmint *n){
    if(!n->used)
        return(INFINITY);
    int bits;
    frexp(n->limb[n->used - 1], &bits);
    bits += (n->used - 1) * LIMB_BITS;
    if(bits + n->twos > LCM_MAX_EXP)
        return(INFINITY);
    int drop = bits > LCM_PREC ? bits - LCM_PREC : 0;
    uint64_t significand = 0;
    for(int i = bits - 1; i >= drop; i--)
        significand = (significand << 1) | ((n->limb[i / LIMB_BITS] >> (i % LIMB_BITS)) & 1);
    /* Round to nearest, ties to even. The low bit of the odd part is set. */
    if(drop){
        int guard = drop - 1;
        if(((n->limb[guard / LIMB_BITS] >> (guard % LIMB_BITS)) & 1)
            && (drop > 1 || (significand & 1)))
            significand++;
    }
    return((t_float)ldexp((double)significand, drop + n->twos));
}

static void lcm_right(t_lcm *x, t_floatarg f){
    if(!isfinite(f)){
        pd_error(x, "[lcm]: expected a finite number");
        return;
    }
    x->x_right = lcm_one;
    lcm_include(&x->x_right, f);
}

static void lcm_list(t_lcm *x, t_symbol *s, int ac, t_atom *av){
    (void)s;
    for(int i = 0; i < ac; i++){
        if(av[i].a_type != A_FLOAT || !isfinite(atom_getfloat(av+i))){
            pd_error(x, "[lcm]: expected finite numbers");
            return;
        }
    }
    if(ac == 2)
        lcm_right(x, atom_getfloat(av+1));
    t_lcmint result = ac > 2 ? lcm_one : x->x_right;
    if(ac){
        int count = ac == 2 ? 1 : ac;
        for(int i = 0; i < count; i++){
            x->x_left = atom_getfloat(av+i);
            if(x->x_left == 0)
                x->x_left = 1;
            lcm_include(&result, x->x_left);
        }
        if(ac > 2)
            x->x_right = result;
    }
    else if(x->x_left != 0)
        lcm_include(&result, x->x_left);
    outlet_float(x->x_obj.ob_outlet, x->x_left == 0 ? 0 : lcm_result(&result));
}

static void *lcm_new(t_floatarg f){
    t_lcm *x = (t_lcm *)pd_new(lcm_class);
    x->x_right = lcm_one;
    lcm_right(x, f);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("right"));
    outlet_new(&x->x_obj, &s_float);
    return(x);
}

void lcm_setup(void){
    lcm_class = class_new(gensym("lcm"), (t_newmethod)lcm_new, 0,
        sizeof(t_lcm), 0, A_DEFFLOAT, 0);
    class_addlist(lcm_class, lcm_list);
    class_addmethod(lcm_class, (t_method)lcm_right, gensym("right"), A_FLOAT, 0);
}
