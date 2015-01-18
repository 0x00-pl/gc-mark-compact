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


typedef struct object_header_t_decl{
	size_t size; // sizeof(object_t) + array_size * sizeof(value)
	size_t type;
	size_t mark;
} object_header_t, object_t;

typedef struct{
	void *ptr;
} object_raw_part_t;
int object_raw_init(object_t *thiz, void* ptr);
int object_raw_halt(object_t *thiz);

typedef struct{
	long int value;
} object_int_part_t;
int object_int_init(object_t *thiz, long int value);
int object_int_halt(object_t *thiz);

typedef struct{
	double value;
} object_float_part_t;
int object_float_init(object_t *thiz, double value);
int object_float_halt(object_t *thiz);

typedef struct{
	char *str;
	size_t size;
} object_str_part_t;
int object_str_init(object_t *thiz, const char* text);
int object_str_halt(object_t *thiz);

typedef struct{
	char *str;
	size_t size;
} object_symbol_part_t;
int object_symbol_init(object_t *thiz, const char* name);
int object_symbol_halt(object_t *thiz);

typedef struct{
	struct object_header_t_decl *ptr;
} object_gc_broken_part_t;
int object_gc_broken_init(object_t *thiz, void* ptr);
int object_gc_broken_halt(object_t *thiz);

typedef struct{
	size_t extra_size; // sizeof(object_extra_t) + sizeof(extra_data)
	size_t extra_type;
} object_extra_part_t;

typedef struct{
	struct object_header_t_decl *ptr;
} object_ref_part_t;
int object_ref_init(object_t *thiz, void* ptr);
int object_ref_halt(object_t *thiz);





int object_halt(object_t *obj);


// object cast
int object_type_check(object_t *obj, enum_object_type_t type);
void* object_part(object_t *obj);
object_raw_part_t       *object_as_raw(object_t *obj);
object_int_part_t       *object_as_int(object_t *obj);
object_float_part_t     *object_as_float(object_t *obj);
object_str_part_t       *object_as_str(object_t *obj);
object_symbol_part_t    *object_as_symbol(object_t *obj);
object_gc_broken_part_t *object_as_gc_broken(object_t *obj);
object_extra_part_t     *object_as_extra(object_t *obj);
object_ref_part_t       *object_as_ref(object_t *obj);




// object size
size_t obj_sizeof_value(enum_object_type_t obj_type);
size_t obj_array_sizeof(enum_object_type_t obj_type, size_t n);
size_t obj_sizeof(enum_object_type_t obj_type);
size_t obj_array_size(object_t *obj);

// object gc support
int obj_mark(object_t *obj, int mark);
int obj_ptr_rebase(object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
int obj_rebase(object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);

#endif
