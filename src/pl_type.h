#ifndef _PL_TYPE_H_
#define _PL_TYPE_H_

#include <limits.h>
#include <stddef.h>
#include "pl_plantform.h"
#include "pl_err.h"

enum type_e{
  TYPE_RAW,
  TYPE_INT,
  TYPE_FLOAT,
  TYPE_STR,
  TYPE_SYMBOL,
  TYPE_REF,
  TYPE_VECTOR,
  TYPE_UNKNOW
};
typedef size_t enum_object_type_t;

// typename
const char *object_typename(enum_object_type_t);


struct object_t_decl;

typedef struct{
  int auto_free;
  void *ptr;
} object_raw_part_t;

typedef long int object_int_value_t;
typedef struct{
  object_int_value_t value;
} object_int_part_t;

typedef double object_float_value_t;
typedef struct{
  object_float_value_t value;
} object_float_part_t;

typedef struct{
  char *str;
  size_t size;
} object_str_part_t;

typedef struct{
  struct object_t_decl *name;
} object_symbol_part_t;

typedef struct{
  size_t count; // data count
  struct object_t_decl *pdata;
} object_vector_part_t;

typedef struct{
  struct object_t_decl *ptr;
} object_ref_part_t;

typedef struct object_header_t_decl{
  void *move_dest;
  size_t size; // sizeof(object_t) + array_size * sizeof(value)
  size_t type;
  size_t mark;
} object_header_t;

typedef struct object_t_decl {
  void *move_dest;
  size_t size; // sizeof(object_t) + array_size * sizeof(value)
  size_t type;
  size_t mark;
  union{
    object_raw_part_t _raw;
    object_int_part_t _int;
    object_float_part_t _float;
    object_str_part_t _str;
    object_symbol_part_t _symbol;
    object_vector_part_t _vector;
    object_ref_part_t _ref;
    char _unknow;
  } part;
} object_t;

// object init/halt
err_t *object_raw_init_nth(err_t **err, object_t *obj, int n, void *ptr, int auto_free);
err_t *object_int_init_nth(err_t **err, object_t *obj, int n, object_int_value_t value);
err_t *object_float_init_nth(err_t **err, object_t *obj, int n, object_float_value_t value);
err_t *object_str_init_nth(err_t **err, object_t *obj, int n, const char* text);
err_t *object_symbol_init_nth(err_t **err, object_t *obj, int n, object_t *name);
err_t *object_vector_init_nth(err_t **err, object_t *obj, int n);
err_t *object_ref_init_nth(err_t **err, object_t *obj, int n, object_t *ptr);

err_t *object_raw_init(err_t **err, object_t *thiz, void *ptr, int auto_free);
err_t *object_int_init(err_t **err, object_t *thiz, object_int_value_t value);
err_t *object_float_init(err_t **err, object_t *thiz, object_float_value_t value);
err_t *object_str_init(err_t **err, object_t *thiz, const char *text);
err_t *object_symbol_init(err_t **err, object_t *thiz, object_t *name);
err_t *object_gc_broken_init(err_t **err, object_t *thiz, object_t *ptr);
err_t *object_vector_init(err_t **err, object_t *thiz);
err_t *object_ref_init(err_t **err, object_t *thiz, object_t *ptr);
err_t *object_init_nth(err_t **err, object_t *obj, int n);
err_t *object_copy_nth(err_t **err, object_t *src, int src_n, object_t *dst, int dst_n);

err_t *object_halt(err_t **err, object_t *obj);



// object cast
err_t *object_type_check(err_t **err, object_t *obj, enum_object_type_t type);
object_float_value_t object_get_float_value(err_t **err, object_t *obj);
object_int_value_t object_get_int_value(err_t **err, object_t *obj);

// object tuple
struct gc_manager_t_decl;
object_t *object_tuple_alloc(err_t **err, struct gc_manager_t_decl *gcm, size_t size);
// err_t *object_member_set_value(err_t **err, object_t *tuple, size_t index, object_t *value);
// void                    *object_member           (err_t **err, object_t *tuple, size_t offset);
// object_raw_part_t       *object_member_raw       (err_t **err, object_t *tuple, size_t offset);
// object_int_part_t       *object_member_int       (err_t **err, object_t *tuple, size_t offset);
// object_float_part_t     *object_member_float     (err_t **err, object_t *tuple, size_t offset);
// object_str_part_t       *object_member_str       (err_t **err, object_t *tuple, size_t offset);
// object_symbol_part_t    *object_member_symbol    (err_t **err, object_t *tuple, size_t offset);
// object_vector_part_t    *object_member_vector    (err_t **err, object_t *tuple, size_t offset);
// object_ref_part_t       *object_member_ref       (err_t **err, object_t *tuple, size_t offset);



// object vecter
struct gc_manager_t_decl;

err_t *object_vector_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item);
err_t *object_vector_ref_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item);

err_t *object_vector_pop(err_t **err, object_t *vec);
err_t *object_vector_top(err_t **err, object_t *vec, object_t *dest);
object_t *object_vector_to_array(err_t **err, object_t *vec, struct gc_manager_t_decl *gcm);
err_t *object_vector_index(err_t **err, object_t *vec, int index, object_t *dest);
object_t *object_vector_ref_index(err_t **err, object_t *vec, int index);
enum_object_type_t object_vector_type(err_t **err, object_t *vec);
size_t object_vector_count(err_t **err, object_t *vec);

//object str
int object_str_eq(object_t *s1, object_t *s2);
int object_num_eq(object_t *o1, object_t *o2);

// object size
size_t object_sizeof(err_t **err, enum_object_type_t obj_type);
size_t object_sizeof_part(err_t **err, enum_object_type_t obj_type);
size_t object_array_sizeof(err_t **err, enum_object_type_t obj_type, size_t n);
size_t object_array_count(err_t **err, object_t *obj);
void* object_array_index(err_t **err, object_t *obj, size_t index);
#define OBJ_ARR_AT(array, _part_name, index) ((&((array)->part._part_name))[index])

object_t *object_array_slice(err_t **err, struct gc_manager_t_decl *gcm, object_t *obj, size_t new_count_beg, size_t new_count_end);

// object verbose
err_t *object_verbose(err_t** err, object_t* obj, int recursive, size_t indentation, size_t limit);
err_t *object_disply(err_t **err, object_t *value);

// object gc support
err_t *object_mark(err_t **err, object_t *obj, size_t mark, size_t limit);
err_t *object_ptr_rebase(err_t **err, object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);
err_t *object_rebase(err_t **err, object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool);

err_t *object_ptr_gc_relink(err_t **err, object_t **pobj, char *dest_pool, size_t dest_pool_maxsize);
err_t *object_gc_relink(err_t **err, object_t *obj, char *dest_pool, size_t dest_pool_maxsize);

err_t *object_move(err_t **err, object_t *obj_old, object_t *obj_new);

#endif
