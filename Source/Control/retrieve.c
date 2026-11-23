// based from cyclone/grab and simplified

#include <m_pd.h>
#include <m_imp.h>
#include <string.h>

struct _outlet{
    t_object        *o_owner;
    struct _outlet  *o_next;
    t_outconnect    *o_connections;
    t_symbol        *o_sym;
};

static t_outconnect *outlet_nextconnection(t_outconnect *last, t_object **destp, int *innop){
    t_inlet *dummy;
    return(obj_nexttraverseoutlet(last, destp, &dummy, innop));
}

static t_class *bindlist_class;

/* Bindlist layout in Pd 0.57 (m_pd.c). */
typedef struct _bindlist{
    t_pd b_pd;
    t_pd **b_vec;
    int b_n;
}t_bindlist;

static int retrieve_stillbound(t_symbol *name, t_pd *who){
    t_pd *bound = name->s_thing;
    if(!bound || *bound != bindlist_class)
        return(bound == who);
    const t_bindlist *list = (t_bindlist *)bound;
    for(int i = 0; i < list->b_n; i++)
        if(list->b_vec[i] == who)
            return(1);
    return(0);
}

/* pdlua binds a receive name to a small proxy, not to the Lua object itself.
   Its class is registered as patchable, so pd_checkobject() takes it for a
   whole t_object although it has no inlets or outlets, and using it as one
   writes past it. What we want is the Lua object (a GUI) that owns it; a bang
   to it has to go through the proxy, as the object takes no messages itself. */
typedef struct _pdlua_proxyreceive{
    t_pd        p_pd;
    t_object   *p_owner; // the pdlua object, which starts with its t_object
    t_symbol   *p_name;
}t_pdlua_proxyreceive;

static int retrieve_islua(t_pd *x){
    return(!strcmp(class_getname(*x), "pdlua proxy receive"));
}

static t_object *retrieve_checkobject(t_pd *x){
    if(retrieve_islua(x))
        return(((t_pdlua_proxyreceive *)x)->p_owner);
    return(pd_checkobject(x));
}

/* A Lua GUI is asked with "retrieve" rather than a bang. It answers with its
   whole state in one message, as [presets] needs it ([multi.vsl] outputs one
   message per slider on a bang), and not to its send name; the abstractions
   these GUIs replace did the same with a [receiver] and a [default] inside. */
static void retrieve_ask(t_pd *x){
    if(retrieve_islua(x))
        pd_typedmess(x, gensym("retrieve"), 0, 0);
    else
        pd_bang(x);
}

typedef struct _retrieve{
    t_object x_obj;
    t_symbol *x_rcv_name;
}t_retrieve;

static t_class *retrieve_class;

/* Point an object's outlet at whatever [retrieve]'s outlet is connected to,
   with connections of its own. Sharing [retrieve]'s connections instead (as
   this used to) shares the cords drawn for them: if the canvas redraws its
   cords while the answer comes in (an object there resizes, say), the cord
   from [retrieve] is moved over to the retrieved object and stays there.
   The object's own connections are stored by the caller, and given back by
   retrieve_giveback. An object without an outlet has nothing to lend, and a
   signal outlet carries no messages to retrieve: changing its connections
   would only rebuild the DSP graph (twice per object, on every retrieve), so
   both are left alone. */
static int retrieve_canlend(t_object *obj){
    t_outlet *objout;
    obj_starttraverseoutlet(obj, &objout, 0);
    return(objout && !obj_issignaloutlet(obj, 0));
}

static void retrieve_lend(t_retrieve *x, t_object *obj){
    t_outlet *op, *objout;
    t_object *to;
    t_inlet *ip;
    int inno;
    if(!retrieve_canlend(obj))
        return;
    t_outconnect *oc = obj_starttraverseoutlet((t_object *)x, &op, 0);
    obj_starttraverseoutlet(obj, &objout, 0);
    objout->o_connections = 0;
    while(oc){
        oc = obj_nexttraverseoutlet(oc, &to, &ip, &inno);
        obj_connect(obj, 0, to, inno);
    }
}

// take down what retrieve_lend made and give the object its own back
static void retrieve_giveback(t_object *obj, t_outconnect *own){
    t_outlet *objout;
    t_object *to;
    t_inlet *ip;
    int inno;
    t_outconnect *oc;
    if(!retrieve_canlend(obj)) // nothing was lent (see retrieve_lend)
        return;
    while((oc = obj_starttraverseoutlet(obj, &objout, 0))){
        obj_nexttraverseoutlet(oc, &to, &ip, &inno);
        obj_disconnect(obj, 0, to, inno);
        if(obj_starttraverseoutlet(obj, &objout, 0) == oc) // can't happen, but don't hang
            break;
    }
    objout->o_connections = own;
}

typedef struct _retrieved{
    t_object *obj;
    t_outconnect *own;
}t_retrieved;

static void retrieve_saveoutlet(t_retrieve *x, t_retrieved *saved, t_object *obj){
    t_outlet *out;
    obj_starttraverseoutlet(obj, &out, 0);
    saved->obj = obj;
    saved->own = out ? out->o_connections : NULL;
    retrieve_lend(x, obj);
}

static void retrieve_from(t_retrieve *x, t_pd *bound){
    t_object *obj = retrieve_checkobject(bound);
    if(!obj)
        return;
    t_symbol *classname = obj->te_g.g_pd->c_name;
    t_retrieved single;
    if(classname != gensym("receive") && classname != gensym("receiver")){
        retrieve_saveoutlet(x, &single, obj);
        retrieve_ask(bound);
        retrieve_giveback(obj, single.own);
        return;
    }

    t_outlet *out;
    t_outconnect *first = obj_starttraverseoutlet(obj, &out, 0);
    t_object *dest;
    int inlet, count = 0;
    for(t_outconnect *oc = first; oc; ){
        oc = outlet_nextconnection(oc, &dest, &inlet);
        if(dest && !inlet)
            count++;
    }
    if(!first)
        return;
    t_retrieved *saved = count > 1 ? getbytes(count * sizeof(*saved)) : &single;
    if(!saved)
        return;
    int n = 0;
    for(t_outconnect *oc = first; oc; ){
        oc = outlet_nextconnection(oc, &dest, &inlet);
        if(dest && !inlet)
            retrieve_saveoutlet(x, saved + n++, dest);
        else if(dest)
            pd_error(x, "[retrieve]: remote receive object must feed leftmost inlet");
    }
    if(n)
        retrieve_ask(bound);
    while(n > 0){
        n--;
        retrieve_giveback(saved[n].obj, saved[n].own);
    }
    if(count > 1)
        freebytes(saved, count * sizeof(*saved));
}

static void retrieve_bang(t_retrieve *x){
    t_symbol *name = x->x_rcv_name;
    if(!name || !name->s_thing || !bindlist_class)
        return;
    if(*name->s_thing != bindlist_class){
        retrieve_from(x, name->s_thing);
        return;
    }

    /* A reply can rebind a receiver or collapse the bindlist to a singleton.
       Keep the original delivery order and skip recipients that have gone. */
    const t_bindlist *list = (t_bindlist *)name->s_thing;
    int count = list->b_n;
    t_pd **receivers = copybytes(list->b_vec, count * sizeof(*receivers));
    if(!receivers)
        return;
    for(int i = 0; i < count; i++)
        if(i == 0 || retrieve_stillbound(name, receivers[i]))
            retrieve_from(x, receivers[i]);
    freebytes(receivers, count * sizeof(*receivers));
}

// set receive name
static void retrieve_set(t_retrieve *x, t_symbol *s){
    if(s && s != &s_)
        x->x_rcv_name = s;
}

static void *retrieve_new(t_symbol *s){
    t_retrieve *x = (t_retrieve *)pd_new(retrieve_class);
    outlet_new((t_object *)x, &s_anything);
    x->x_rcv_name = (s && s != &s_) ? s : 0;
    return(x);
}

void retrieve_setup(void){
    t_symbol *s = gensym("retrieve");
    retrieve_class = class_new(s, (t_newmethod)retrieve_new, 0,
        sizeof(t_retrieve), 0, A_DEFSYMBOL, 0);
    class_addbang(retrieve_class, retrieve_bang);
    class_addmethod(retrieve_class, (t_method)retrieve_set, gensym("set"), A_SYMBOL, 0);
    if(!bindlist_class){
        t_class *c = retrieve_class;
        pd_bind(&retrieve_class, s);
        pd_bind(&c, s);
        if(!s->s_thing || !(bindlist_class = *s->s_thing)
	    || bindlist_class->c_name != gensym("bindlist"))
            pd_error(retrieve_class, "retrieve: failure to initialize retrieve name");
	pd_unbind(&c, s);
	pd_unbind(&retrieve_class, s);
    }
}
