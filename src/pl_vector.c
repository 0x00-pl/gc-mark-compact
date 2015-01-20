#include "pl_vector.h"

#include <stdlib.h>
#include <string.h>
#include "pl_err.h"


// vector data resize
int object_resize(object_t **pobj, gc_manager_t *gcm, size_t new_size){
  PL_CHECK_RET_BEG
  PL_ASSERT_NOT_NULL(pobj);
  PL_ASSERT_NOT_NULL(*pobj);
  object_t *new_array = NULL;
  size_t new_array_size;
  size_t copy_size;
  size_t old_size = object_array_size(*pobj);
  
  if(old_size == new_size) {return 0;}
  
  new_array = gc_manager_object_array_alloc(gcm, (*pobj)->type, new_size);
  PL_ASSERT_NOT_NULL(new_array);
  
  new_array_size = new_array->size;
  
  copy_size = (*pobj)->size < new_array->size ? (*pobj)->size : new_array->size;
  
  memcpy(new_array, *pobj, copy_size);
  
  new_array->size = new_array_size;
  
  *pobj = new_array;
  PL_CHECK_RET_END
}

// vector member
object_vector_t *object_vector_alloc(gc_manager_t *gcm){
  object_vector_t *ret = NULL;
  object_t *size = NULL;
  ret = gc_manager_object_array_alloc(gcm, TYPE_REF, 2);
  PL_ASSERT_EX(ret!=NULL, PL_PRINT_LOCATION, return ret);
  
  // size
  PL_ASSERT_EX((size=gc_manager_object_alloc(gcm, TYPE_INT))!=NULL, PL_PRINT_LOCATION, return NULL);
  PL_ASSERT_EX(object_int_init(size, 0)>=0, PL_PRINT_LOCATION, return NULL);
  PL_ASSERT_EX(object_member_set_value(ret, 0, size)>=0, PL_PRINT_LOCATION, return NULL);
  
  // data
  PL_ASSERT_EX(object_member_set_value(ret, 1, NULL)>=0, PL_PRINT_LOCATION, return NULL);
  
  return ret;
}

object_int_part_t *object_vector_size(object_t *vector){
  return object_member_int(vector, 0);
}
object_ref_part_t *object_vector_data(object_t *vector){
  return (object_ref_part_t*)object_array_index(vector, 1);
}
size_t object_vector_maxsize(object_t *vector){
  return object_array_size(object_vector_data(vector)->ptr);
}
enum_object_type_t object_vector_data_type(object_t *vector){
  if(object_vector_data(vector)->ptr == NULL) {return TYPE_UNKNOW;}
  return object_vector_data(vector)->ptr->type;
}

// vector push/pop
int object_vector_push(object_t *vector, gc_manager_t *gcm, object_t *value){
  PL_CHECK_RET_BEG
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
  
  if(value == NULL) {return -1;}
  
  PL_ASSERT_NOT_NULL(object_vector_size(vector));
  if(object_vector_size(vector)->value == 0){
    // reset vector data type
    object_vector_data(vector)->ptr = gc_manager_object_array_alloc(gcm, value->type, 1);
  }
  
  m_size = object_vector_size(vector);  
  m_data = object_vector_data(vector);
  maxsize = object_vector_maxsize(vector);
  PL_ASSERT(m_size!=NULL && m_data!=NULL && maxsize!=0, err_null());
  
  // vector data type check
  PL_ASSERT(object_vector_data_type(vector)==value->type, err_typecheck());
  
  // vector maxsize check
  if(m_size->value+1 >= maxsize){
    PL_CHECK_RET(object_resize(&(object_vector_data(vector)->ptr), gcm, maxsize * 2));
  }
  
  // copy into vector
  src = object_array_index(value, 0);
  dst = object_array_index(m_data->ptr, m_size->value);
  PL_ASSERT(src!=NULL && dst!=NULL, err_null());
  
  memcpy(dst, src, object_sizeof_part(value->type));
  
  m_size->value += 1;
  
  PL_CHECK_RET_END
}

int object_vector_top(object_t *vector, object_t *out_value){
  PL_CHECK_RET_BEG
  object_int_part_t *m_size;
  object_ref_part_t *m_data;
  void *src;
  void *dst;
  
  PL_ASSERT_NOT_NULL(m_size = object_vector_size(vector));
  PL_ASSERT(m_size->value >= 0, err_out_of_range());
  
  PL_ASSERT_NOT_NULL(m_data = object_vector_data(vector));
  
  if(out_value != NULL){
    PL_ASSERT_NOT_NULL(m_data->ptr);
    out_value->size = object_sizeof(m_data->ptr->type);
    out_value->type = object_vector_data_type(vector);
    PL_ASSERT(out_value->size!=0 && out_value->type!=TYPE_UNKNOW, err_typecheck());
    
    
    src = object_array_index(m_data->ptr, m_size->value - 1);
    dst = object_array_index(out_value, 0);
    PL_ASSERT(src!=NULL && dst!=NULL, err_null());
    memcpy(dst, src, object_sizeof_part(object_vector_data_type(vector)));
  }
  PL_CHECK_RET_END
}

int object_vector_pop(object_t *vector, object_t *out_value){
  PL_CHECK_RET_BEG
  object_int_part_t *m_size = NULL;
  
  m_size = object_vector_size(vector);
  PL_ASSERT_NOT_NULL(m_size);
  PL_ASSERT(m_size->value >= 0, err_out_of_range());
  
  PL_CHECK_RET(object_vector_top(vector, out_value));
  
  m_size->value -= 1;

  PL_CHECK_RET_END
}

int object_vector_realloc(object_t *vector, gc_manager_t *gcm){
  PL_CHECK_RET_BEG
  object_int_part_t *m_size = NULL;
  object_ref_part_t *m_data = NULL;
  
  PL_ASSERT_NOT_NULL(m_size = object_vector_size(vector));
  PL_ASSERT(m_size->value < 0, err_out_of_range());
  PL_ASSERT_NOT_NULL(m_data = object_vector_data(vector));
  
  PL_CHECK_RET(object_resize(&m_data->ptr, gcm, m_size->value));
  
  PL_CHECK_RET_END
}

