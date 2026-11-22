#ifndef ELSE_COLOR_H
#define ELSE_COLOR_H

#include <m_pd.h>

// Read _validate as atoms: Pd's typed dispatcher groups symbols before floats.
static inline t_symbol *else_parse_color_reply(t_pd *object, int argc, const t_atom *argv,
    int *valid, int *which){
    if(argc < (which ? 3 : 2) || argv[0].a_type != A_FLOAT ||
        argv[1].a_type != A_SYMBOL || (which && argv[2].a_type != A_FLOAT)){
        pd_error(object, "bad arguments for message '_validate' to object '%s'",
            class_getname(pd_class(object)));
        return NULL;
    }
    *valid = (int)atom_getfloat(argv);
    if(which)
        *which = (int)atom_getfloat(argv + 2);
    return atom_getsymbol(argv + 1);
}

#endif
