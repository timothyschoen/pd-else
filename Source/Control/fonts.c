// porres 2026

#include <m_pd.h>
#include <g_canvas.h>

static t_class *fonts_class;

typedef struct _fonts{
    t_object    x_obj;
    t_symbol    *x_id;
    t_canvas    *x_cv;
}t_fonts;

static void fonts__output_fonts(t_fonts *x, t_symbol *s, int ac, t_atom *av){
    (void)s;
    for(int i = 0; i < ac; i++)
        outlet_symbol(x->x_obj.ob_outlet, atom_getsymbol(av + i));
}

static void fonts_bang(t_fonts *x) {
    sys_vgui("fonts__output_fonts %s\n", x->x_id->s_name);
}

static void fonts__output_pdinfo(t_fonts *x, t_symbol *s, int ac, t_atom *av){
    (void)s;
    if(ac >= 2)
        outlet_list(x->x_obj.ob_outlet, &s_list, 2, av);
}

static void fonts_pdinfo(t_fonts *x){
    int size = glist_getfont(x->x_cv);
    sys_vgui("set fontlist [get_font_for_size %d]\n", size);
    sys_vgui("set fontfamily [lindex $fontlist 0]\n");
    sys_vgui("set fontstyle [lindex $fontlist 2]\n");
    sys_vgui("set quoted_family [string map {{ } {\\ }} $fontfamily]\n");
    sys_vgui("set cmd [concat %s _output_pdinfo $quoted_family $fontstyle \\;]\n",
        x->x_id->s_name);
    sys_vgui("pdsend $cmd\n");
}

static void fonts_free(t_fonts *x) {
    pd_unbind(&x->x_obj.ob_pd, x->x_id);
}

static void *fonts_new(void){
    t_fonts *x = (t_fonts *)pd_new(fonts_class);
    x->x_cv = canvas_getrootfor(canvas_getcurrent());
    char buf[64];
    sprintf(buf, "#%lx", (t_int)x);
    x->x_id = gensym(buf);
    pd_bind(&x->x_obj.ob_pd, x->x_id);
    outlet_new(&x->x_obj, &s_symbol);
    return(x);
}

void fonts_setup(void){
    fonts_class = class_new(gensym("fonts"), (t_newmethod)fonts_new,
        (t_method)fonts_free, sizeof(t_fonts), CLASS_DEFAULT, 0);
    class_addbang(fonts_class, fonts_bang);
    class_addmethod(fonts_class, (t_method)fonts_pdinfo, gensym("pdfont"), 0);
    class_addmethod(fonts_class, (t_method)fonts__output_fonts,
        gensym("_output_fonts"), A_GIMME, 0);
    class_addmethod(fonts_class, (t_method)fonts__output_pdinfo,
        gensym("_output_pdinfo"), A_GIMME, 0);
    sys_gui("proc fonts__output_fonts {obj_id} {\n"
        "    set fonts [lsort [font families]]\n"
        "    foreach font $fonts {\n"
        "        set quoted_font [string map {{ } {\\ }} $font]\n"
        "        pdsend [concat $obj_id _output_fonts $quoted_font \\;]\n"
        "    }\n"
        "}\n");
}
