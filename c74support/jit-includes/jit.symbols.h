/* 
	jit.symbols.h

	Copyright 2001-2004 - Cycling '74
	Joshua Kit Clayton jkc@cycling74.com
		
*/

#ifndef __JIT_SYMBOLS_H__
#define __JIT_SYMBOLS_H__

JIT_EX_DATA t_symbol *_jit_sym_nothing;
JIT_EX_DATA t_symbol *_jit_sym_new;
JIT_EX_DATA t_symbol *_jit_sym_free;
JIT_EX_DATA t_symbol *_jit_sym_classname;
JIT_EX_DATA t_symbol *_jit_sym_getname;
JIT_EX_DATA t_symbol *_jit_sym_getmethod;
JIT_EX_DATA t_symbol *_jit_sym_get;
JIT_EX_DATA t_symbol *_jit_sym_set;
JIT_EX_DATA t_symbol *_jit_sym_register;
JIT_EX_DATA t_symbol *_jit_sym_char;
JIT_EX_DATA t_symbol *_jit_sym_long;
JIT_EX_DATA t_symbol *_jit_sym_float32;
JIT_EX_DATA t_symbol *_jit_sym_float64;
JIT_EX_DATA t_symbol *_jit_sym_symbol;
JIT_EX_DATA t_symbol *_jit_sym_pointer;
JIT_EX_DATA t_symbol *_jit_sym_object;
JIT_EX_DATA t_symbol *_jit_sym_atom;
JIT_EX_DATA t_symbol *_jit_sym_list;
JIT_EX_DATA t_symbol *_jit_sym_type;
JIT_EX_DATA t_symbol *_jit_sym_dim;
JIT_EX_DATA t_symbol *_jit_sym_planecount;
JIT_EX_DATA t_symbol *_jit_sym_val;
JIT_EX_DATA t_symbol *_jit_sym_plane;
JIT_EX_DATA t_symbol *_jit_sym_cell;
JIT_EX_DATA t_symbol *_jit_sym_jit_matrix;
JIT_EX_DATA t_symbol *_jit_sym_class_jit_matrix;
JIT_EX_DATA t_symbol *_jit_sym_togworld;
JIT_EX_DATA t_symbol *_jit_sym_fromgworld;
JIT_EX_DATA t_symbol *_jit_sym_frommatrix;
JIT_EX_DATA t_symbol *_jit_sym_class_jit_attribute;
JIT_EX_DATA t_symbol *_jit_sym_jit_attribute;
JIT_EX_DATA t_symbol *_jit_sym_jit_attr_offset;
JIT_EX_DATA t_symbol *_jit_sym_jit_attr_offset_array;
JIT_EX_DATA t_symbol *_jit_sym_rebuilding;
JIT_EX_DATA t_symbol *_jit_sym_modified;
JIT_EX_DATA t_symbol *_jit_sym_lock;
JIT_EX_DATA t_symbol *_jit_sym_setinfo;
JIT_EX_DATA t_symbol *_jit_sym_setinfo_ex;
JIT_EX_DATA t_symbol *_jit_sym_getinfo;
JIT_EX_DATA t_symbol *_jit_sym_data;
JIT_EX_DATA t_symbol *_jit_sym_getdata;
JIT_EX_DATA t_symbol *_jit_sym_outputmatrix;
JIT_EX_DATA t_symbol *_jit_sym_clear;
JIT_EX_DATA t_symbol *_jit_sym_err_calculate;
JIT_EX_DATA t_symbol *_jit_sym_max_jit_classex;
JIT_EX_DATA t_symbol *_jit_sym_setall;
JIT_EX_DATA t_symbol *_jit_sym_chuck;
JIT_EX_DATA t_symbol *_jit_sym_getsize;
JIT_EX_DATA t_symbol *_jit_sym_getindex;
JIT_EX_DATA t_symbol *_jit_sym_objptr2index;
JIT_EX_DATA t_symbol *_jit_sym_append;
JIT_EX_DATA t_symbol *_jit_sym_insertindex;
JIT_EX_DATA t_symbol *_jit_sym_deleteindex;
JIT_EX_DATA t_symbol *_jit_sym_chuckindex;
JIT_EX_DATA t_symbol *_jit_sym_makearray;
JIT_EX_DATA t_symbol *_jit_sym_reverse;
JIT_EX_DATA t_symbol *_jit_sym_rotate;
JIT_EX_DATA t_symbol *_jit_sym_shuffle;
JIT_EX_DATA t_symbol *_jit_sym_swap;
JIT_EX_DATA t_symbol *_jit_sym_findfirst;
JIT_EX_DATA t_symbol *_jit_sym_findall;
JIT_EX_DATA t_symbol *_jit_sym_methodall;
JIT_EX_DATA t_symbol *_jit_sym_methodindex;
JIT_EX_DATA t_symbol *_jit_sym_sort;
JIT_EX_DATA t_symbol *_jit_sym_matrix_calc;
JIT_EX_DATA t_symbol *_jit_sym_genframe;
JIT_EX_DATA t_symbol *_jit_sym_filter;
JIT_EX_DATA t_symbol *_jit_sym_jit_mop;
JIT_EX_DATA t_symbol *_jit_sym_newcopy;
JIT_EX_DATA t_symbol *_jit_sym_jit_linklist;
JIT_EX_DATA t_symbol *_jit_sym_inputcount;
JIT_EX_DATA t_symbol *_jit_sym_outputcount;
JIT_EX_DATA t_symbol *_jit_sym_getinput;
JIT_EX_DATA t_symbol *_jit_sym_getoutput;
JIT_EX_DATA t_symbol *_jit_sym_getinputlist;
JIT_EX_DATA t_symbol *_jit_sym_getoutputlist;
JIT_EX_DATA t_symbol *_jit_sym_ioname;
JIT_EX_DATA t_symbol *_jit_sym_matrixname;
JIT_EX_DATA t_symbol *_jit_sym_outputmode;
JIT_EX_DATA t_symbol *_jit_sym_matrix;
JIT_EX_DATA t_symbol *_jit_sym_getmatrix;
JIT_EX_DATA t_symbol *_jit_sym_typelink;
JIT_EX_DATA t_symbol *_jit_sym_dimlink;
JIT_EX_DATA t_symbol *_jit_sym_planelink;
JIT_EX_DATA t_symbol *_jit_sym_restrict_type;
JIT_EX_DATA t_symbol *_jit_sym_restrict_planecount;
JIT_EX_DATA t_symbol *_jit_sym_restrict_dim;
JIT_EX_DATA t_symbol *_jit_sym_special;
JIT_EX_DATA t_symbol *_jit_sym_getspecial;
JIT_EX_DATA t_symbol *_jit_sym_adapt;
JIT_EX_DATA t_symbol *_jit_sym_decorator;
JIT_EX_DATA t_symbol *_jit_sym_frommatrix_trunc;
JIT_EX_DATA t_symbol *_jit_sym_ioproc;
JIT_EX_DATA t_symbol *_jit_sym_getioproc;
JIT_EX_DATA t_symbol *_jit_sym_name;
JIT_EX_DATA t_symbol *_jit_sym_types;
JIT_EX_DATA t_symbol *_jit_sym_minplanecount;
JIT_EX_DATA t_symbol *_jit_sym_maxplanecount;
JIT_EX_DATA t_symbol *_jit_sym_mindim;
JIT_EX_DATA t_symbol *_jit_sym_maxdim;
JIT_EX_DATA t_symbol *_jit_sym_gl_points;
JIT_EX_DATA t_symbol *_jit_sym_gl_lines;
JIT_EX_DATA t_symbol *_jit_sym_gl_line_strip;
JIT_EX_DATA t_symbol *_jit_sym_gl_line_loop;
JIT_EX_DATA t_symbol *_jit_sym_gl_triangles;
JIT_EX_DATA t_symbol *_jit_sym_gl_tri_strip;
JIT_EX_DATA t_symbol *_jit_sym_gl_tri_fan;
JIT_EX_DATA t_symbol *_jit_sym_gl_quads;
JIT_EX_DATA t_symbol *_jit_sym_gl_quad_strip;
JIT_EX_DATA t_symbol *_jit_sym_gl_polygon;
JIT_EX_DATA t_symbol *_jit_sym_gl_tri_grid;
JIT_EX_DATA t_symbol *_jit_sym_gl_quad_grid;
JIT_EX_DATA t_symbol *_jit_sym_err_lockout_stack;
JIT_EX_DATA t_symbol *_jit_sym_class_jit_namespace;
JIT_EX_DATA t_symbol *_jit_sym_jit_namespace;
JIT_EX_DATA t_symbol *_jit_sym_findsize;
JIT_EX_DATA t_symbol *_jit_sym_attach;
JIT_EX_DATA t_symbol *_jit_sym_detach;
JIT_EX_DATA t_symbol *_jit_sym_add;
JIT_EX_DATA t_symbol *_jit_sym_replace;
JIT_EX_DATA t_symbol *_jit_sym_gettype;
JIT_EX_DATA t_symbol *_jit_sym_ob_sym;
JIT_EX_DATA t_symbol *_jit_sym_resolve_name;
JIT_EX_DATA t_symbol *_jit_sym_resolve_raw;
JIT_EX_DATA t_symbol *_jit_sym_notifyall;

#endif // __JIT_SYMBOLS_H__
