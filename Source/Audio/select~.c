// porres

#include <m_pd.h>
#include <string.h>

static t_class *select_class;

typedef struct _select{
    t_object   x_obj;
    int        x_n;
    int        x_sel;
    int        x_nchans; // output channels at the last dsp
    int       *x_chs;    // input channels at the last dsp
    t_sample **x_ins;
}t_select;

static int select_index(t_select *x){
    return((x->x_sel > x->x_n ? x->x_n : x->x_sel) - 1);
}

// Rebuilding all DSP for every switch is quadratic in patches with many of these, so only do so
// when the output needs a different channel count
static void select_float(t_select *x, t_floatarg f){
    x->x_sel = f;
    int i = select_index(x);
    if((i >= 0 ? x->x_chs[i] : 1) != x->x_nchans)
        canvas_update_dsp();
}

static t_int *select_perform(t_int *w){
    t_select *x = (t_select *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]) * x->x_nchans, i = select_index(x);
    if(i >= 0 && x->x_chs[i] == x->x_nchans)
        memmove(out, x->x_ins[i], n * sizeof(t_sample));
    else
        memset(out, 0, n * sizeof(t_sample));
    return(w+4);
}

static void select_dsp(t_select *x, t_signal **sp){
    int n = x->x_n, i = select_index(x);
    for(int j = 0; j < n; j++)
        x->x_ins[j] = sp[j]->s_vec, x->x_chs[j] = sp[j]->s_nchans;
    signal_setmultiout(&sp[n], x->x_nchans = i >= 0 ? sp[i]->s_nchans : 1);
    dsp_add(select_perform, 3, x, sp[n]->s_vec, sp[0]->s_n);
}

static void select_free(t_select *x){
    freebytes(x->x_chs, x->x_n * sizeof(*x->x_chs));
    freebytes(x->x_ins, x->x_n * sizeof(*x->x_ins));
}

static void *select_new(t_symbol *s, int ac, t_atom *av){
    s = NULL;
    t_select *x = (t_select *)pd_new(select_class);
    x->x_n = 2;
    x->x_sel = 0;
    if(ac){
        x->x_n = atom_getfloat(av);
        ac--, av++;
    }
    if(ac){
        x->x_sel = atom_getfloat(av);
        ac--, av++;
    }
    if(x->x_n < 1)
        x->x_n = 1;
    if(x->x_sel < 0)
        x->x_n = 0;
    x->x_chs = (int *)getbytes(x->x_n * sizeof(*x->x_chs));
    x->x_ins = (t_sample **)getbytes(x->x_n * sizeof(*x->x_ins));
    for(int i = 1; i < x->x_n; i++)
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, &s_signal);
    return(x);
}

void select_tilde_setup(void){
    select_class = class_new(gensym("select~"), (t_newmethod)select_new,
        (t_method)select_free, sizeof(t_select), CLASS_MULTICHANNEL, A_GIMME, 0);
    class_addfloat(select_class, select_float);
    class_addmethod(select_class, nullfn, gensym("signal"), 0);
    class_addmethod(select_class, (t_method)select_dsp, gensym("dsp"), 0);
}
