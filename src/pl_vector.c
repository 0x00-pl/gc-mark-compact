#include "pl_vector.h"

#include <stdlib.h>
#include <string.h>
#include "pl_err.h"


// vector data resize
err_t *object_resize(err_t **err, object_t **pobj, gc_manager_t *gcm, size_t new_size){
  PL_ASSERT_NOT_NULL(pobj);
  PL_ASSERT_NOT_NULL(*pobj);
  object_t *new_array = NULL;
  size_t new_array_size;
  size_t copy_size;
  size_t old_size = object_array_size(err, *pobj);
  
  if(old_size == new_size) {return 0;}
  
  new_array = gc_manager_object_array_alloc(err, gcm, (*pobj)->type, new_size); PL_CHECK;
  
  new_array_size = new_array->size;
  
  copy_size = (*pobj)->size < new_array->size ? (*pobj)->size : new_array->size;
  
  memcpy(new_array, *pobj, copy_size);
  
  new_array->size = new_array_size;
  
  *pobj = new_array;
  PL_FUNC_END
  return *err;
}

// vector member
object_vector_t *object_vector_alloc(err_t **err, gc_manager_t *gcm){
  object_vector_t *ret = NULL;
  object_t *size = NULL;
  ret = gc_manager_object_array_alloc(err, gcm, TYPE_REF, 2); PL_CHECK;
  
  // size
  size=gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  
  object_int_init(err, size, 0); PL_CHECK;
  object_member_set_value(err, ret, 0, size); PL_CHECK;
  
  // data
  object_member_set_value(err, ret, 1, NULL); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}

object_int_part_t *object_vector_size(err_t **err, object_t *vector){
  return object_member_int(err, vector, 0);
}
object_ref_part_t *object_vector_data(err_t **err, object_t *vector){
  return (object_ref_part_t*)object_array_index(err, vector, 1);
}
size_t object_vector_maxsize(err_t **err, object_t *vector){
  return object_array_size(err, object_vector_data(err, vector)->ptr);
}
enum_object_type_t object_vector_data_type(err_t **err, object_t *vector){
  object_vector_data(err, vector); PL_CHECK;
  PL_ASSERT_NOT_NULL(object_vector_data(err, vector)->ptr);
  PL_FUNC_END_EX(,return TYPE_UNKNOW);
  return object_vector_data(err, vector)->ptr->type;
}

// vector push/pop
err_t *object_vector_push(err_t **err, object_t *vector, gc_manager_t *gcm, object_t *value){   
  PL_ASSERT_NOT_NULL(value);
  long int old_size;
  long int required_size;
  size_t maxsize;
  enum_object_type_t object_type;
  object_int_part_t *m_size;
  object_ref_part_t *m_data;
  void *arr;
  void *src;
  void *dst;
  
  object_vector_size(err, vector); PL_CHECK;
  if(object_vector_size(err, vector)->value == 0){
    // reset vector data type
    object_vector_data(err, vector)->ptr = gc_manager_object_array_alloc(err, gcm, value->type, 1);
  }
  
  m_size = object_vector_size(err, vector);  
  m_data = object_vector_data(err, vector);
  maxsize = object_vector_maxsize(err, vector);
  PL_ASSERT(m_size!=NULL && m_data!=NULL && maxsize!=0, err_null);
  
  // vector data type check
  PL_ASSERT(object_vector_data_type(err, vector)==value->type, err_typecheck);
  
  // vector maxsize check
  if(m_size->value+1 >= maxsize){
    object_resize(err, &(object_vector_data(err, vector)->ptr), gcm, maxsize * 2); PL_CHECK 
  }
  
  // copy into vector
  src = object_array_index(err, value, 0);
  dst = object_array_index(err, m_data->ptr, m_size->value);
  PL_ASSERT(src!=NULL && dst!=NULL, err_null);
  
  memcpy(dst, src, object_sizeof_part(err, value->type));
  
  m_size->value += 1;
  
  PL_FUNC_END
  return *err;
}

err_t *object_vector_top(err_t **err, object_t *vector, object_t *out_value){
   
  object_int_part_t *m_size;
  object_ref_part_t *m_data;
  void *src;
  void *dst;
  
  m_size = object_vector_size(err, vector); PL_CHECK;
  PL_ASSERT(m_size->value >= 0, err_out_of_range);
  
  m_data = object_vector_data(err, vector); PL_CHECK;
  
  if(out_value != NULL){
    PL_ASSERT_NOT_NULL(m_data->ptr);
    out_value->size = object_sizeof(err, m_data->ptr->type); PL_CHECK;
    out_value->type = object_vector_data_type(err, vector); PL_CHECK;
    PL_ASSERT(out_value->size!=0 && out_value->type!=TYPE_UNKNOW, err_typecheck);
    
    
    src = object_array_index(err, m_data->ptr, m_size->value - 1); PL_CHECK;
    dst = object_array_index(err, out_value, 0); PL_CHECK;
    PL_ASSERT(src!=NULL && dst!=NULL, err_null);
    memcpy(dst, src, object_sizeof_part(err, object_vector_data_type(err, vector)));  PL_CHECK;
  }
  PL_FUNC_END
  return *err;
}

err_t *object_vector_pop(err_t **err, object_t *vector, object_t *out_value){
   
  object_int_part_t *m_size = NULL;
  
  m_size = object_vector_size(err, vector);  PL_CHECK;
  
  PL_ASSERT(m_size->value >= 0, err_out_of_range);
  
  object_vector_top(err, vector, out_value); PL_CHECK 
  
  m_size->value -= 1;

  PL_FUNC_END
  return *err;
}

err_t *object_vector_realloc(err_t **err, object_t *vector, gc_manager_t *gcm){
   
  object_int_part_t *m_size = NULL;
  object_ref_part_t *m_data = NULL;
  
  m_size = object_vector_size(err, vector);  PL_CHECK;
  PL_ASSERT(m_size->value < 0, err_out_of_range);
  m_data = object_vector_data(err, vector); PL_CHECK;
  
  object_resize(err, &m_data->ptr, gcm, m_size->value);  PL_CHECK; 
  
  PL_FUNC_END
  return *err;
}

