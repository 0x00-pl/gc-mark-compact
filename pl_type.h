#ifndef _PL_TYPE_H_
#define _PL_TYPE_H_

#include <stddef.h>

enum type_e{
	TYPE_RAW,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STR,
	TYPE_SYMBOL,
	TYPE_GC_BROKEN,
	TYPE_EXTRA,
	TYPE_REF,
};
typedef size_t enum_object_type_t;

struct object_header_t_decl;

typedef struct{
	void *ptr;
} object_raw_t;

typedef struct{
	long int value;
} object_int_t;

typedef struct{
	double val;
} object_float_t;

typedef struct{
	char *str;
} object_str_t;

typedef struct{
	char *str;
} object_symbol_t;

typedef struct{
	struct object_header_t_decl *ptr;
} object_gc_broken_t;

typedef struct{
	size_t extra_size; // sizeof(object_extra_t) + sizeof(extra_data)
	size_t extra_type;
} object_extra_t;

typedef struct{
	struct object_header_t_decl *ptr;
} object_ref_t;



typedef struct object_header_t_decl{
	size_t size; // sizeof(object_t) + array_size * sizeof(value)
	size_t type;
	size_t mark;
} object_header_t, object_t;

int obj_free(object_t *obj);

size_t obj_sizeof_value(enum_object_type_t obj_type);

size_t obj_array_sizeof(enum_object_type_t obj_type, size_t n);

size_t obj_sizeof(enum_object_type_t obj_type);

size_t gc_object_array_size(object_t *obj);

void* gc_obj_value(object_t *obj);

int obj_mark(object_t *obj, int mark);

int obj_ptr_rebase(object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);

int obj_rebase(object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);

#endif
