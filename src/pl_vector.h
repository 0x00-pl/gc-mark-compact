#ifndef _PL_VECTOR_H_
#define _PL_VECTOR_H_

#include "pl_type.h"
#include "pl_gc.h"

typedef object_t object_vector_t;

object_vector_t *object_vector_alloc(gc_manager_t *gcm);

// vector data resize
int object_resize(object_t **pobj, gc_manager_t *gcm, size_t new_size);

// vector member
object_int_part_t *object_vector_size(object_t *vector);
object_ref_part_t *object_vector_data(object_t *vector);
size_t object_vector_maxsize(object_t *vector);
enum_object_type_t object_vector_data_type(object_t *vector);

// vector push/pop
int object_vector_push(object_t *vector, gc_manager_t *gcm, object_t *value);
int object_vector_top(object_t *vector, object_t *out_value);
int object_vector_pop(object_t *vector, object_t *out_value);
int object_vector_realloc(object_t *vector, gc_manager_t *gcm);


#endif

