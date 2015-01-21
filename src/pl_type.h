#ifndef _PL_TYPE_H_
#define _PL_TYPE_H_

#include <stddef.h>
#include "pl_err.h"

enum type_e{
	TYPE_RAW,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STR,
	TYPE_SYMBOL,
	TYPE_GC_BROKEN,
	TYPE_EXTRA,
	TYPE_REF,
	TYPE_UNKNOW
};
typedef size_t enum_object_type_t;

// typename
const char *object_typename(enum_object_type_t);

typedef struct object_header_t_decl{
	size_t size; // sizeof(object_t) + array_size * sizeof(value)
	size_t type;
	size_t mark;
} object_header_t, object_t;

typedef struct{
	void *ptr;
} object_raw_part_t;

typedef struct{
	long int value;
} object_int_part_t;

typedef struct{
	double value;
} object_float_part_t;

typedef struct{
	char *str;
	size_t size;
} object_str_part_t;

typedef struct{
	char *str;
	size_t size;
} object_symbol_part_t;

typedef struct{
	struct object_header_t_decl *ptr;
} object_gc_broken_part_t;

typedef struct{
	size_t extra_size; // sizeof(object_extra_t) + sizeof(extra_data)
	size_t extra_type;
} object_extra_part_t;

typedef struct{
	struct object_header_t_decl *ptr;
} object_ref_part_t;


// object init/halt
err_t *object_halt(err_t **err, object_t *obj);
err_t *object_raw_init(err_t **err, object_t *thiz, void* ptr);
err_t *object_raw_halt(err_t **err, object_t *thiz);
err_t *object_int_init(err_t **err, object_t *thiz, long int value);
err_t *object_int_halt(err_t **err, object_t *thiz);
err_t *object_float_init(err_t **err, object_t *thiz, double value);
err_t *object_float_halt(err_t **err, object_t *thiz);
err_t *object_str_init(err_t **err, object_t *thiz, const char* text);
err_t *object_str_halt(err_t **err, object_t *thiz);
err_t *object_symbol_init(err_t **err, object_t *thiz, const char* name);
err_t *object_symbol_halt(err_t **err, object_t *thiz);
err_t *object_gc_broken_init(err_t **err, object_t *thiz, void* ptr);
err_t *object_gc_broken_halt(err_t **err, object_t *thiz);
err_t *object_ref_init(err_t **err, object_t *thiz, void* ptr);
err_t *object_ref_halt(err_t **err, object_t *thiz);


// object copy
err_t *object_copy_init(err_t **err, object_t *src, object_t *dst);


// object cast
err_t *object_type_check(err_t **err, object_t *obj, enum_object_type_t type);
void* object_part(err_t **err, object_t *obj);
object_raw_part_t       *object_as_raw       (err_t **err, object_t *obj);
object_int_part_t       *object_as_int       (err_t **err, object_t *obj);
object_float_part_t     *object_as_float     (err_t **err, object_t *obj);
object_str_part_t       *object_as_str       (err_t **err, object_t *obj);
object_symbol_part_t    *object_as_symbol    (err_t **err, object_t *obj);
object_gc_broken_part_t *object_as_gc_broken (err_t **err, object_t *obj);
object_extra_part_t     *object_as_extra     (err_t **err, object_t *obj);
object_ref_part_t       *object_as_ref       (err_t **err, object_t *obj);


// object tuple
struct gc_manager_t_decl;
object_t *object_tuple_alloc(err_t **err, struct gc_manager_t_decl *gcm, size_t size);
err_t *object_member_set_value(err_t **err, object_t *tuple, size_t index, object_t *value);
void                    *object_member           (err_t **err, object_t *tuple, size_t offset);
object_raw_part_t       *object_member_raw       (err_t **err, object_t *tuple, size_t offset);
object_int_part_t       *object_member_int       (err_t **err, object_t *tuple, size_t offset);
object_float_part_t     *object_member_float     (err_t **err, object_t *tuple, size_t offset);
object_str_part_t       *object_member_str       (err_t **err, object_t *tuple, size_t offset);
object_symbol_part_t    *object_member_symbol    (err_t **err, object_t *tuple, size_t offset);
object_gc_broken_part_t *object_member_gc_broken (err_t **err, object_t *tuple, size_t offset);
object_extra_part_t     *object_member_extra     (err_t **err, object_t *tuple, size_t offset);
object_ref_part_t       *object_member_ref       (err_t **err, object_t *tuple, size_t offset);


// object size
size_t object_sizeof(err_t **err, enum_object_type_t obj_type);
size_t object_sizeof_part(err_t **err, enum_object_type_t obj_type);
size_t object_array_sizeof(err_t **err, enum_object_type_t obj_type, size_t n);
size_t object_array_size(err_t **err, object_t *obj);
void* object_array_index(err_t **err, object_t *obj, size_t index);


// object gc support
err_t *object_mark(err_t **err, object_t *obj, int mark);
err_t *object_ptr_rebase(err_t **err, object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
err_t *object_rebase(err_t **err, object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
err_t *object_move(err_t **err, object_t *obj_old, object_t *obj_new);
err_t *object_fix_gc_broken(err_t **err, object_t *obj);
#endif
