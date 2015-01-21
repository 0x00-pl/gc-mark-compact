#ifndef _PL_VECTOR_H_
#define _PL_VECTOR_H_

#include "pl_type.h"
#include "pl_gc.h"

typedef object_t object_vector_t;

object_vector_t *object_vector_alloc(err_t **err, gc_manager_t *gcm);

// vector data resize
err_t *object_resize(err_t **err, object_t **pobj, gc_manager_t *gcm, size_t new_size);

// vector member
object_int_part_t *object_vector_size(err_t **err, object_t *vector);
object_ref_part_t *object_vector_data(err_t **err, object_t *vector);
size_t object_vector_maxsize(err_t **err, object_t *vector);
enum_object_type_t object_vector_data_type(err_t **err, object_t *vector);

// vector push/pop
err_t *object_vector_push(err_t **err, object_t *vector, gc_manager_t *gcm, object_t *value);
err_t *object_vector_top(err_t **err, object_t *vector, object_t *out_value);
err_t *object_vector_pop(err_t **err, object_t *vector, object_t *out_value);
err_t *object_vector_realloc(err_t **err, object_t *vector, gc_manager_t *gcm);


#endif

