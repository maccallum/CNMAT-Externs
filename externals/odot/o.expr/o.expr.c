/*
Written by John MacCallum, The Center for New Music and Audio Technologies,
University of California, Berkeley.  Copyright (c) 2011, The Regents of
the University of California (Regents). 
Permission to use, copy, modify, distribute, and distribute modified versions
of this software and its documentation without fee and without a signed
licensing agreement, is hereby granted, provided that the above copyright
notice, this paragraph and the following two paragraphs appear in all copies,
modifications, and distributions.

IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
NAME: o.expr
DESCRIPTION: C-like expressions that operate on OSC bundles
AUTHORS: John MacCallum
COPYRIGHT_YEARS: 2011
SVN_REVISION: $LastChangedRevision: 587 $
VERSION 0.0: First try
VERSION 1.0: Uses flex and bison to do the lexing/parsing
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*/

#include "../odot_version.h"

#ifdef NAME
#undef NAME
#endif


#if defined (OCOND)

#define OMAX_DOC_NAME "o.cond"
#define OMAX_DOC_SHORT_DESC "Route an OSC packet out an outlet based on the results of an expression."
#define OMAX_DOC_LONG_DESC "o.cond creates one outlet for each expression separated by a semicolon. The bundle will come out the outlet that corresponds to the first expression that evaluates to true or non-zero."
#define OMAX_DOC_INLETS_DESC (char *[]){"OSC packet containing addresses that the expression will be applied to."}
#define OMAX_DOC_SEEALSO (char *[]){"o.expr", "o.if", "o.when", "o.unless", "o.callpatch", "expr", "jit.expr"}

#elif defined (OIF)

#define OMAX_DOC_NAME "o.if"
#define OMAX_DOC_SHORT_DESC "Route an OSC packet based on the results of an expression."
#define OMAX_DOC_LONG_DESC "o.if routs the incoming bundle out the left outlet if the result of the expression is true or non-zero, and out the right outlet otherwise."
#define OMAX_DOC_INLETS_DESC (char *[]){"OSC packet containing addresses that the expression will be applied to."}
#define OMAX_DOC_OUTLETS_DESC (char *[]){"Input OSC FullPacket if the expression returns true or non-zero", "Input OSC FullPacket if the expression returns false or zero"}
#define OMAX_DOC_SEEALSO (char *[]){"o.expr", "o.cond", "o.when", "o.unless", "o.callpatch", "expr", "jit.expr"}

#elif defined (OWHEN)

#define OMAX_DOC_NAME "o.when"
#define OMAX_DOC_SHORT_DESC "Passes the bundle through if the result of the expression is true or non-zero."
#define OMAX_DOC_LONG_DESC "o.when behaves like o.if with only the left-most outlet (i.e. the \"then\" outlet"
#define OMAX_DOC_INLETS_DESC (char *[]){"OSC packet containing addresses that the expression will be applied to."}
#define OMAX_DOC_OUTLETS_DESC (char *[]){"Input OSC packet if the expression returns true or non-zero."}
#define OMAX_DOC_SEEALSO (char *[]){"o.if", "o.cond", "o.unless", "o.expr", "expr", "jit.expr"}

#elif defined (OUNLESS)

#define OMAX_DOC_NAME "o.unless"
#define OMAX_DOC_SHORT_DESC "Passes the bundle through if the result of the expression is false or zero."
#define OMAX_DOC_LONG_DESC "o.unless behaves like o.if with only the right-most outlet (i.e. the \"else\" outlet."
#define OMAX_DOC_INLETS_DESC (char *[]){"OSC packet containing addresses that the expression will be applied to."}
#define OMAX_DOC_OUTLETS_DESC (char *[]){"Input OSC packet if the expression returns false or zero."}
#define OMAX_DOC_SEEALSO (char *[]){"o.if", "o.cond", "o.when", "o.expr", "expr", "jit.expr"}

#else

#define OMAX_DOC_NAME "o.expr"
#define OMAX_DOC_SHORT_DESC "Evaluate a C-like expression containing OSC addresses."
#define OMAX_DOC_LONG_DESC "When it reveives a packet, o.expr substitutes any OSC addresses contained in the expression for the values to which they are bound in the incoming packet.  The expression is then evaluated and the resulting bundle, containing any side effects of the expression, is output."
#define OMAX_DOC_INLETS_DESC (char *[]){"OSC packet containing addresses that the expression will be applied to."}
#define OMAX_DOC_OUTLETS_DESC (char *[]){"The OSC packet containing the results of the expression."}
#define OMAX_DOC_SEEALSO (char *[]){"o.callpatch", "o.if", "o.cond", "o.when", "o.unless", "expr", "jit.expr"}

#endif

#define NAME OMAX_DOC_NAME

#ifndef WIN_VERSION
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif
#include "ext.h"
#include "ext_obex.h"
#include "ext_obex_util.h"
//#include "jpatcher_api.h" 
//#include "jgraphics.h"
#include "osc.h"
#include "osc_expr.h"
#include "osc_expr_parser.h"
#include "osc_mem.h"
#include "osc_atom_u.h"
#include "osc_error.h"
#include "omax_util.h"
#include "omax_doc.h"

//#define __OSC_PROFILE__
#include "osc_profile.h"

double rdtsc_cps;

typedef struct _oexpr{
	t_object ob;
#if defined (OIF) || defined (OCOND)
	void **outlets;
#else
	void *outlet;
#endif
#ifdef OCOND
	int num_exprs;
	char **outlets_desc;
#endif
	t_osc_expr *function_graph;
	//t_jrgba background_color, frame_color, text_color;
} t_oexpr;

void *oexpr_class;

void oexpr_fullPacket(t_oexpr *x, long len, long ptr);
/*
void oexpr_set(t_oexpr *x, t_symbol *s, long ac, t_atom *av);
void oexpr_select(t_oexpr *x);
void oexpr_doselect(t_oexpr *x);
long oexpr_key(t_oexpr *x, t_object *patcherview, long keycode, long modifiers, long textcharacter);
long oexpr_keyfilter(t_oexpr *x, t_object *patcherview, long *keycode, long *modifiers, long *textcharacter);
void oexpr_enter(t_oexpr *x);
void oexpr_gettext(t_oexpr *x);
*/
void oexpr_output_bundle(t_oexpr *x);
//void oexpr_mousedown(t_oexpr *x, t_object *patcherview, t_pt pt, long modifiers);
//void oexpr_mouseup(t_oexpr *x, t_object *patcherview, t_pt pt, long modifiers);

void oexpr_free(t_oexpr *x);
void *oexpr_new(t_symbol *msg, short argc, t_atom *argv);


void oexpr_fullPacket(t_oexpr *x, long len, long ptr){
#if defined (OIF) || defined (OCOND) || defined (OWHEN) || defined (OUNLESS)
	t_osc_atom_ar_u *argv = NULL;
	// we don't actually want to do this copy here.  we need to 
	// have another version of omax_expr_funcall that doesn't do 
	// assignment

	char *copy = NULL;
	long copylen = 0;
	char alloc = 0;
	if(strncmp((char *)ptr, "#bundle\0", 8)){
		osc_bundle_s_wrapMessage(len, (char *)ptr, &copylen, &copy, &alloc);
	}else{
		copy = (char *)osc_mem_alloc(len);
		memcpy(copy, (char *)ptr, len);
	}

#if defined (OIF)
	int ret = osc_expr_funcall(x->function_graph, &len, &copy, &argv);
	if(ret){
		omax_util_outletOSC(x->outlets[1], len, (char *)ptr);
	}else{
		int i;
		for(i = 0; i < osc_atom_array_u_getLen(argv); i++){
			if(osc_atom_u_getDouble(osc_atom_array_u_get(argv, i)) == 0){
				omax_util_outletOSC(x->outlets[1], len, (char *)ptr);
				goto out;
			}
		}
		omax_util_outletOSC(x->outlets[0], len, (char *)ptr);
	}
 out:
	if(argv){
		osc_atom_array_u_free(argv);
	}
	if(copy){
		osc_mem_free(copy);
	}
#elif defined (OUNLESS)
	int ret = osc_expr_funcall(x->function_graph, &len, &copy, &argv);
	if(ret){
		omax_util_outletOSC(x->outlet, len, (char *)ptr);
	}else{
		int i;
		for(i = 0; i < osc_atom_array_u_getLen(argv); i++){
			if(osc_atom_u_getDouble(osc_atom_array_u_get(argv, i)) == 0){
				omax_util_outletOSC(x->outlet, len, (char *)ptr);
				goto out;
			}
		}
	}
 out:
	if(argv){
		osc_atom_array_u_free(argv);
	}
	if(copy){
		osc_mem_free(copy);
	}
#elif defined (OWHEN)
	int ret = osc_expr_funcall(x->function_graph, &len, &copy, &argv);
	if(ret){
	}else{
		int i;
		for(i = 0; i < osc_atom_array_u_getLen(argv); i++){
			if(osc_atom_u_getDouble(osc_atom_array_u_get(argv, i)) == 0){
				goto out;
			}
		}
		omax_util_outletOSC(x->outlet, len, (char *)ptr);
	}
 out:
	if(argv){
		osc_atom_array_u_free(argv);
	}
	if(copy){
		osc_mem_free(copy);
	}
#elif defined (OCOND)
	t_osc_expr *f = x->function_graph;
	int j = 0;
	while(f){
		int ret = osc_expr_funcall(f, &len, &copy, &argv);
		if(ret){
			continue;
		}else{
			int i;
			int fail = 0;
			for(i = 0; i < osc_atom_array_u_getLen(argv); i++){
				if(osc_atom_u_getDouble(osc_atom_array_u_get(argv, i)) == 0){
					fail = 1;
					break;
				}
			}
			if(argv){
				osc_mem_free(argv);
				argv = NULL;
			}
			if(!fail){
				omax_util_outletOSC(x->outlets[j], len, (char *)ptr);
				goto out;
			}
		}
		f = osc_expr_next(f);
		j++;
	}
	omax_util_outletOSC(x->outlets[j], len, (char *)ptr);
 out:
	if(argv){
		osc_atom_array_u_free(argv);
	}
	if(copy){
		free(copy);
	}
#endif

#else
	// o.expr

	// we need to make a copy incase the expression contains assignment that will
	// alter the bundle.
	// the copy needs to use memory allocated with osc_mem_alloc in case the 
	// bundle has to be resized during assignment
	char *copy = NULL;
	long copylen = len;
	if(strncmp((char *)ptr, "#bundle\0", 8)){
		char alloc = 0;
		osc_bundle_s_wrapMessage(len, (char *)ptr, &copylen, &copy, &alloc);
	}else{
		copy = (char *)osc_mem_alloc(len);
		memcpy(copy, (char *)ptr, len);
	}
	int ret = 0;
	t_osc_expr *f = x->function_graph;
	if(!f){
		t_osc_atom_ar_u *argv = NULL;
		osc_expr_evalLexExprsInBndl(&copylen, &copy, &argv);
	}else{
		while(f){
			//int argc = 0;
			t_osc_atom_ar_u *argv = NULL;
			ret = osc_expr_funcall(f, &copylen, &copy, &argv);
			if(argv){
				osc_atom_array_u_free(argv);
			}
			if(ret){
				break;
			}
			f = osc_expr_next(f);
		}
	}
	if(ret){
		omax_util_outletOSC(x->outlet, len, (char *)ptr);
	}else{
		omax_util_outletOSC(x->outlet, copylen, copy);
	}
	if(copy){
		osc_mem_free(copy);
	}
#endif
}
/*
void oexpr_paint(t_oexpr *x, t_object *patcherview){
	t_rect rect;
	t_jgraphics *g = (t_jgraphics *)patcherview_get_jgraphics(patcherview);
	jbox_get_rect_for_view((t_object *)x, patcherview, &rect);

	jgraphics_set_source_jrgba(g, &(x->background_color));
	jgraphics_rectangle(g, 0., 0., rect.width, rect.height);
	jgraphics_fill(g);

	jgraphics_set_source_jrgba(g, &(x->frame_color));
	jgraphics_set_line_width(g, 2.);
	jgraphics_rectangle(g, 0., 0., rect.width, rect.height);
	jgraphics_stroke(g);

	jgraphics_move_to(g, 3, 0);
	jgraphics_line_to(g, 3, rect.height);
	jgraphics_stroke(g);
}

void oexpr_select(t_oexpr *x){
	defer(x, (method)oexpr_doselect, 0, 0, 0);
}

void oexpr_doselect(t_oexpr *x){
	t_object *p = NULL; 
	object_obex_lookup(x,gensym("#P"), &p);
	if (p) {
		t_atom rv; 
		long ac = 1; 
		t_atom av[1]; 
		atom_setobj(av, x); 
		object_method_typed(p, gensym("selectbox"), ac, av, &rv); 
	}
}


long oexpr_key(t_oexpr *x, t_object *patcherview, long keycode, long modifiers, long textcharacter){
	char buff[256];
	buff[0] = textcharacter;  // we know this is just a simple char
	buff[1] = 0; 
	object_method(patcherview, gensym("insertboxtext"), x, buff);
	jbox_redraw((t_jbox *)x);

	return 1; 
}

long oexpr_keyfilter(t_oexpr *x, t_object *patcherview, long *keycode, long *modifiers, long *textcharacter){
	t_atom arv;
	long rv = 1;
	long k = *keycode;
	
	if (k == JKEY_TAB || k == JKEY_ESC) {
		object_method_typed(patcherview, gensym("endeditbox"), 0, NULL, &arv); 
		rv = 0;		// don't pass those keys to oexpr
	}
	return rv;
}

// enter is triggerd at "endeditbox time"
void oexpr_enter(t_oexpr *x){
	oexpr_gettext(x);
}

void oexpr_gettext(t_oexpr *x){
	long size	= 0;
	char *text	= NULL;
	t_object *textfield = jbox_get_textfield((t_object *)x);
	object_method(textfield, gensym("gettextptr"), &text, &size);
	{
		size = strlen(text); // the value returned in text doesn't make sense
		if(size == 0){
			return;
		}
		t_osc_expr *f = NULL;
		osc_expr_parser_parseString(text, &f);
		if(!f){
			object_error((t_object *)x, "error parsing %s\n", text);
			return;
		}
		if(x->function_graph){
			osc_expr_free(x->function_graph);
		}
		x->function_graph = f;
	}
}

void oexpr_mousedown(t_oexpr *x, t_object *patcherview, t_pt pt, long modifiers){
        //textfield_set_textmargins(jbox_get_textfield((t_object *)x), 4, 4, 2, 2);
	//jbox_redraw((t_jbox *)x);
}

void oexpr_mouseup(t_oexpr *x, t_object *patcherview, t_pt pt, long modifiers){
        //textfield_set_textmargins(jbox_get_textfield((t_object *)x), 3, 3, 3, 3);
	//jbox_redraw((t_jbox *)x);
	//oexpr_output_bundle(x);
}
*/
void oexpr_postFunctionGraph(t_oexpr *fg){
	char *buf = NULL;
	long len = 0;
	t_osc_expr *f = fg->function_graph;
	//while(f){
	osc_expr_formatFunctionGraph(f, &len, &buf);
	// the modulo op '%' gets consumed as a format character with cycling's post() function.
	// so go through and escape each one with another %
	char buf2[len * 2];
	char *r = buf, *w = buf2;
	int i;
	for(i = 0; i < len; i++){
		if(*r == '%'){
			*w++ = '%';
		}
		*w++ = *r++;
	}
	*w = '\0';
	post("%s", buf2);
	//f = osc_expr_next(f);
	//
	if(buf){
		osc_mem_free(buf);
	}
}

void oexpr_postFunctionTable(t_oexpr *fg){
	char *buf = NULL;
	long len = 0;
	osc_expr_formatFunctionTable(&len, &buf);
	char *ptr1 = buf, *ptr2 = buf;
	while(*ptr2){
		if(*ptr2 == '\n'){
			*ptr2 = '\0';
			post("%s", ptr1);
			ptr1 = ptr2 + 1;
			ptr2++;
		}
		ptr2++;
	}
	if(buf){
		osc_mem_free(buf);
	}
}

void oexpr_bang(t_oexpr *x){
	char buf[16];
	strncpy(buf, "#bundle\0", 8);
	oexpr_fullPacket(x, 16, (long)buf);
}

void oexpr_anything(t_oexpr *x, t_symbol *msg, int argc, t_atom *argv){
	object_error((t_object *)x, "nope");
}

void oexpr_postConstants(t_oexpr *x){
	int i;
	for(i = 0; i < sizeof(osc_expr_constsym) / sizeof(t_osc_expr_const_rec); i++){
		post("%s: %s (%f)", osc_expr_constsym[i].name, osc_expr_constsym[i].docstring, osc_expr_constsym[i].val);
	}
}

void oexpr_postFunctions(t_oexpr *x){
	int i;
	for(i = 0; i < sizeof(osc_expr_funcsym) / sizeof(t_osc_expr_rec); i++){
		if(osc_expr_funcsym[i].arity < 0){
			post("%s(...): %s", osc_expr_funcsym[i].name, osc_expr_funcsym[i].docstring);
		}else if(osc_expr_funcsym[i].arity == 0){
			post("%s(): %s", osc_expr_funcsym[i].name, osc_expr_funcsym[i].docstring);
		}else{
			char buf[256];
			char *ptr = buf;
			ptr += sprintf(ptr, "%s(", osc_expr_funcsym[i].name);
			int j;
			for(j = 0; j < osc_expr_funcsym[i].arity; j++){
				ptr += sprintf(ptr, "arg%d ", j + 1);
			}
			*(--ptr) = '\0';
			post("%s): %s", buf, osc_expr_funcsym[i].docstring);
		}
	}
}

void oexpr_documentation(t_oexpr *x, t_symbol *func)
{
	t_osc_expr_rec *rec = osc_expr_lookupFunction(func->s_name);
	if(rec){
		post("%s(): %s", rec->name, rec->docstring);
	}
}

void oexpr_doc(t_oexpr *x)
{
#ifdef OCOND
	_omax_doc_outletDoc(x->outlets[0],			
			    OMAX_DOC_NAME,		
			    OMAX_DOC_SHORT_DESC,	
			    OMAX_DOC_LONG_DESC,		
			    OMAX_DOC_NINLETS,		
			    OMAX_DOC_INLETS_DESC,	
			    x->num_exprs + 1,
			    x->outlets_desc,
			    OMAX_DOC_NUM_SEE_ALSO_REFS,	
			    OMAX_DOC_SEEALSO);
#else
#ifdef OIF
	omax_doc_outletDoc(x->outlets[0]);
#else
	omax_doc_outletDoc(x->outlet);
#endif
#endif
}

void oexpr_assist(t_oexpr *x, void *b, long io, long num, char *buf){
#ifdef OCOND
	_omax_doc_assist(io, num, buf, OMAX_DOC_NINLETS, OMAX_DOC_INLETS_DESC, x->num_exprs + 1, x->outlets_desc);
#else
	omax_doc_assist(io, num, buf);
#endif
}

void oexpr_free(t_oexpr *x){
	//jbox_free((t_jbox *)x);
	osc_expr_free(x->function_graph);
#if defined (OIF) || defined (OCOND)
	if(x->outlets){
		free(x->outlets);
	}
#endif
#ifdef OCOND
	int i;
	for(i = 0; i < x->num_exprs; i++){
		if(x->outlets_desc){
			osc_mem_free(x->outlets_desc);
		}
	}
	osc_mem_free(x->outlets_desc);
#endif
}

t_max_err oexpr_notify(t_oexpr *x, t_symbol *s, t_symbol *msg, void *sender, void *data){
	return MAX_ERR_NONE;
	t_symbol *attrname;

        if(msg == gensym("attr_modified")){
		attrname = (t_symbol *)object_method((t_object *)data, gensym("getname"));
	}
	return MAX_ERR_NONE;
}

void *oexpr_new(t_symbol *msg, short argc, t_atom *argv){
	t_oexpr *x;
	/*
	t_dictionary *d = NULL; 
 	long boxflags; 
    
	// box setup 
	if(!(d = object_dictionaryarg(argc, argv))){ 
		return NULL; 
	} 

	boxflags = 0 
		| JBOX_DRAWFIRSTIN 
		| JBOX_NODRAWBOX
		| JBOX_DRAWINLAST
		| JBOX_TRANSPARENT  
		//      | JBOX_NOGROW
		//| JBOX_GROWY
		//| JBOX_GROWBOTH
		//      | JBOX_HILITE
		//| JBOX_BACKGROUND
		//| JBOX_DRAWBACKGROUND
		//      | JBOX_NOFLOATINSPECTOR
		//      | JBOX_MOUSEDRAGDELTA
		| JBOX_TEXTFIELD
		;
	*/
	if(x = (t_oexpr *)object_alloc(oexpr_class)){
		//jbox_new((t_jbox *)x, boxflags, argc, argv); 
 		//x->ob.b_firstin = (void *)x; 
		osc_error_setHandler(omax_util_liboErrorHandler);
		t_osc_expr *f = NULL;
		int haspound = 0;
		if(argc){
			char buf[65536];
			char *ptr = buf;
			int i;
			for(i = 0; i < argc; i++){
				switch(atom_gettype(argv + i)){
				case A_LONG:
					ptr += sprintf(ptr, "%ld ", atom_getlong(argv + i));
					break;
				case A_FLOAT:
					ptr += sprintf(ptr, "%f ", atom_getfloat(argv + i));
					break;
				case A_SYM:
					{
						char *s = atom_getsym(argv + i)->s_name;
						int len = strlen(s);
						
						if(*s == '#'){
							if(len > 1){
								if(s[1] > 47 && s[1] < 58){
									haspound++;
									ptr += sprintf(ptr, "%d ", 0);
									break;
								}
							}
						}
						ptr += sprintf(ptr, "%s ", s);
					}
					break;
				}
			}
			if(1){//if(!haspound){
				TIMER_START(foo, rdtsc_cps);
				int ret = osc_expr_parser_parseString(buf, &f);
				TIMER_STOP(foo, rdtsc_cps);
				TIMER_PRINTF(foo);
				TIMER_SNPRINTF(foo, buff);
#ifdef __OSC_PROFILE__
				post("%s\n", buff);
#endif
				if(!f || ret){
					object_error((t_object *)x, "error parsing %s\n", buf);
					return NULL;
				}
				x->function_graph = f;
			}else{
				x->function_graph = NULL;
			}
		}

		int n = 0;
		while(f){
			n++;
			f = osc_expr_next(f);
		}

#if defined (OIF)
		if(n == 0 || n > 1){
			object_error((t_object *)x, "invalid number of expressions: %d", n);
			return NULL;
		}
		x->outlets = malloc(2 * sizeof(void *));
		x->outlets[1] = outlet_new((t_object *)x, "FullPacket");
		x->outlets[0] = outlet_new((t_object *)x, "FullPacket");
#elif defined (OUNLESS) || defined (OWHEN)
		if(n == 0 || n > 1){
			object_error((t_object *)x, "invalid number of expressions: %d", n);
			return NULL;
		}
		x->outlet = outlet_new((t_object *)x, "FullPacket");
#elif defined (OCOND)
		x->num_exprs = n;
		// implicit 't' as the last condition
		x->outlets = malloc(n + 1 * sizeof(void *));
		int i;
		for(i = n; i >= 0; i--){
			x->outlets[i] = outlet_new((t_object *)x, "FullPacket");
		}

		x->outlets_desc = (char **)osc_mem_alloc((x->num_exprs + 1) * sizeof(char *));
		for(i = 0; i < x->num_exprs; i++){
			x->outlets_desc[i] = (char *)osc_mem_alloc(128);
			sprintf(x->outlets_desc[i], "Input OSC packet if expression %d returns true or non-zero", i+1);
		}
		x->outlets_desc[x->num_exprs] = (char *)osc_mem_alloc(128);
		sprintf(x->outlets_desc[x->num_exprs], "Input OSC packet if all expressions return false or zero");
#else
		x->outlet = outlet_new((t_object *)x, "FullPacket");
#endif
		/*
		if(argc){
			omax_scanner_scan_atom_array(attr_args_offset(argc, argv), argv, &(x->argclex), &(x->argvlex));
			x->function_graph = NULL;
			omax_expr_parse(&(x->function_graph), x->argclex, x->argvlex);
		}
		*/

		//attr_dictionary_process(x, d); 
		/*
		t_object *textfield = jbox_get_textfield((t_object *)x);
		if(textfield){
			object_attr_setchar(textfield, gensym("editwhenunlocked"), 1);
			textfield_set_editonclick(textfield, 0);
			textfield_set_textmargins(textfield, 7, 3, 3, 3);
			textfield_set_textcolor(textfield, &(x->text_color));
		}

 		jbox_ready((t_jbox *)x);
		oexpr_gettext(x);
		*/
	}
		   	
	return x;
}

int main(void){
	t_class *c = class_new(NAME, (method)oexpr_new, (method)oexpr_free, sizeof(t_oexpr), 0L, A_GIMME, 0);

	//c->c_flags |= CLASS_FLAG_NEWDICTIONARY; 
 	//jbox_initclass(c, JBOX_TEXTFIELD | JBOX_FIXWIDTH | JBOX_FONTATTR); 
    
	//class_addmethod(c, (method)oexpr_paint, "paint", A_CANT, 0); 

	//osc_set_mem((void *)sysmem_newptr, sysmem_freeptr, (void *)sysmem_resizeptr);
	class_addmethod(c, (method)oexpr_fullPacket, "FullPacket", A_LONG, A_LONG, 0);
	class_addmethod(c, (method)oexpr_assist, "assist", A_CANT, 0);
	//class_addmethod(c, (method)oexpr_notify, "notify", A_CANT, 0);
	class_addmethod(c, (method)oexpr_anything, "anything", A_GIMME, 0);
	class_addmethod(c, (method)oexpr_bang, "bang", 0);

	class_addmethod(c, (method)oexpr_postFunctions, "post-functions", 0);
	class_addmethod(c, (method)oexpr_postConstants, "post-constants", 0);
	class_addmethod(c, (method)oexpr_postFunctionGraph, "post-function-graph", 0);
	class_addmethod(c, (method)oexpr_postFunctionTable, "post-function-table", 0);
	class_addmethod(c, (method)oexpr_documentation, "func-doc", A_SYM, 0);
	class_addmethod(c, (method)oexpr_documentation, "function-documentation", A_SYM, 0);

	class_addmethod(c, (method)oexpr_doc, "doc", 0);
	/*
	class_addmethod(c, (method)oexpr_key, "key", A_CANT, 0);
	class_addmethod(c, (method)oexpr_keyfilter, "keyfilter", A_CANT, 0);
	class_addmethod(c, (method)oexpr_enter, "enter", A_CANT, 0);
	class_addmethod(c, (method)oexpr_select, "select", 0);

	class_addmethod(c, (method)oexpr_mousedown, "mousedown", A_CANT, 0);
	class_addmethod(c, (method)oexpr_mouseup, "mouseup", A_CANT, 0);


 	CLASS_ATTR_RGBA(c, "background_color", 0, t_oexpr, background_color); 
 	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "background_color", 0, ".87 .87 .87 1."); 
 	CLASS_ATTR_STYLE_LABEL(c, "background_color", 0, "rgba", "Background Color"); 
	CLASS_ATTR_CATEGORY(c, "background_color", 0, "Color");

 	CLASS_ATTR_RGBA(c, "frame_color", 0, t_oexpr, frame_color); 
 	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "frame_color", 0, "0. 0. 0. 1."); 
 	CLASS_ATTR_STYLE_LABEL(c, "frame_color", 0, "rgba", "Frame Color"); 
	CLASS_ATTR_CATEGORY(c, "frame_color", 0, "Color");

 	CLASS_ATTR_RGBA(c, "text_color", 0, t_oexpr, text_color); 
 	CLASS_ATTR_DEFAULT_SAVE_PAINT(c, "text_color", 0, "0. 0. 0. 1."); 
 	CLASS_ATTR_STYLE_LABEL(c, "text_color", 0, "rgba", "Text Color"); 
	CLASS_ATTR_CATEGORY(c, "text_color", 0, "Color");

	CLASS_ATTR_DEFAULT(c, "rect", 0, "0. 0. 150., 18.");
	*/
	class_register(CLASS_BOX, c);
	oexpr_class = c;

	common_symbols_init();

	rdtsc_cps = RDTSC_CYCLES_PER_SECOND;

	ODOT_PRINT_VERSION;

	return 0;
}

