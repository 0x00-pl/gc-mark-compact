#include "pl_type.h"

#include <stdlib.h>
#include <string.h>

#include "pl_gc.h"
#include "pl_err.h"



// typename
const char *object_typename(enum_object_type_t type){
  switch(type){
  case TYPE_RAW       : return "TYPE_RAW";
  case TYPE_INT       : return "TYPE_INT";
  case TYPE_FLOAT     : return "TYPE_FLOAT";
  case TYPE_STR       : return "TYPE_STR";
  case TYPE_SYMBOL    : return "TYPE_SYMBOL";
  case TYPE_VECTOR    : return "TYPE_VECTOR";
  case TYPE_REF       : return "TYPE_REF";
  default             : return "TYPE_UNKNOW";
  }
  return "TYPE_UNKNOW";
}

// object init/halt

err_t *object_raw_init_nth(err_t **err, object_t *obj, int n, void *ptr, int auto_free){
  object_type_check(err, obj, TYPE_RAW); PL_CHECK;
  OBJ_ARR_AT(obj, _raw, n).ptr = ptr;
  OBJ_ARR_AT(obj, _raw, n).auto_free = auto_free;
  PL_FUNC_END
  return *err;
}
err_t *object_int_init_nth(err_t **err, object_t *obj, int n, object_int_value_t value){
  object_type_check(err, obj, TYPE_INT); PL_CHECK;
  OBJ_ARR_AT(obj, _int, n).value = value;
  PL_FUNC_END
  return *err;
}
err_t *object_float_init_nth(err_t **err, object_t *obj, int n, object_float_value_t value){
  object_type_check(err, obj, TYPE_FLOAT); PL_CHECK;
  OBJ_ARR_AT(obj, _float, n).value = value;
  PL_FUNC_END
  return *err;
}
err_t *object_str_init_nth(err_t **err, object_t *obj, int n, const char* text){
  object_type_check(err, obj, TYPE_STR); PL_CHECK;
  char *text_copy = NULL;
  OBJ_ARR_AT(obj, _str, n).size = strlen(text);
  text_copy = malloc(OBJ_ARR_AT(obj, _str, n).size+1);
  strncpy(text_copy, text, OBJ_ARR_AT(obj, _str, n).size);
  text_copy[OBJ_ARR_AT(obj, _str, n).size] = '\0';
  OBJ_ARR_AT(obj, _str, n).str = text_copy;
  PL_FUNC_END
  return *err;
}
err_t *object_symbol_init_nth(err_t **err, object_t *obj, int n, object_t *name){
  object_type_check(err, obj, TYPE_SYMBOL); PL_CHECK;
  OBJ_ARR_AT(obj, _symbol, n).name = name;
  PL_FUNC_END
  return *err;
}
err_t *object_vector_init_nth(err_t **err, object_t *obj, int n){
  object_type_check(err, obj, TYPE_VECTOR); PL_CHECK;
  OBJ_ARR_AT(obj, _vector, n).count = 0;
  OBJ_ARR_AT(obj, _vector, n).pdata = NULL;
  PL_FUNC_END
  return *err;
}
err_t *object_ref_init_nth(err_t **err, object_t *obj, int n, object_t *ptr){
  object_type_check(err, obj, TYPE_REF); PL_CHECK;
  OBJ_ARR_AT(obj, _ref, n).ptr = ptr;
  PL_FUNC_END
  return *err;
}
err_t *object_init_nth(err_t **err, object_t *obj, int n){
  switch(obj->type){
    case TYPE_RAW:
      object_raw_init_nth(err, obj, n, NULL, 0); PL_CHECK;
      break;
    case TYPE_INT:
      object_int_init_nth(err, obj, n, 0); PL_CHECK;
      break;
    case TYPE_FLOAT:
      object_float_init_nth(err, obj, n, 0); PL_CHECK;
      break;
    case TYPE_STR:
      object_str_init_nth(err, obj, n, "undefined"); PL_CHECK;
      break;
    case TYPE_SYMBOL:
      object_symbol_init_nth(err, obj, n, NULL); PL_CHECK;
      break;
    case TYPE_REF:
      object_ref_init_nth(err, obj, n, NULL); PL_CHECK;
      break;
    case TYPE_VECTOR:
      object_vector_init_nth(err, obj, n); PL_CHECK;
      break;
    default:
      PL_ASSERT(0, err_typecheck);
  }
  PL_FUNC_END
  return *err;
}

err_t *object_raw_init(err_t **err, object_t *obj, void *ptr, int auto_free){
  return object_raw_init_nth(err, obj, 0, ptr, auto_free);
}
err_t *object_int_init(err_t **err, object_t *obj, object_int_value_t value){
  return object_int_init_nth(err, obj, 0, value);
}
err_t *object_float_init(err_t **err, object_t *obj, object_float_value_t value){
  return object_float_init_nth(err, obj, 0, value);
}
err_t *object_str_init(err_t **err, object_t *obj, const char* text){
  return object_str_init_nth(err, obj, 0, text);
}
err_t *object_symbol_init(err_t **err, object_t *obj, object_t *name){
  return object_symbol_init_nth(err, obj, 0, name);
}
err_t *object_vector_init(err_t **err, object_t *obj){
  return object_vector_init_nth(err, obj, 0);
}
err_t *object_ref_init(err_t **err, object_t *obj, object_t *ptr){
  return object_ref_init_nth(err, obj, 0, ptr);
}

err_t *object_copy_nth(err_t **err, object_t *src, int src_n, object_t *dst, int dst_n){
  object_type_check(err, dst, src->type); PL_CHECK;
  switch(src->type){
    case TYPE_RAW:
      object_raw_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_raw,src_n).ptr, OBJ_ARR_AT(src,_raw,src_n).auto_free); PL_CHECK;
      break;
    case TYPE_INT:
      object_int_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_int,src_n).value); PL_CHECK;
      break;
    case TYPE_FLOAT:
      object_float_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_float,src_n).value); PL_CHECK;
      break;
    case TYPE_STR:
      object_str_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_str,src_n).str); PL_CHECK;
      break;
    case TYPE_SYMBOL:
      object_symbol_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_symbol,src_n).name); PL_CHECK;
      break;
    case TYPE_VECTOR:
      object_vector_init_nth(err, dst, dst_n); PL_CHECK;
      break;
    case TYPE_REF:
      object_ref_init_nth(err, dst, dst_n, OBJ_ARR_AT(src,_ref,src_n).ptr); PL_CHECK;
      break;
    default:
      break;
  }
  PL_FUNC_END
  return *err;
}


err_t *object_halt_nth(err_t **err, object_t *obj, int n){
  switch(obj->type){
  case TYPE_RAW:
    if(OBJ_ARR_AT(obj, _raw, n).auto_free){
      free(OBJ_ARR_AT(obj, _raw, n).ptr);
    }
    break;
  case TYPE_STR:
    free(OBJ_ARR_AT(obj, _str, n).str);
    break;
  case TYPE_REF:
    OBJ_ARR_AT(obj, _ref, n).ptr = NULL;
    break;
  case TYPE_VECTOR:
    OBJ_ARR_AT(obj, _vector, n).count = 0;
    OBJ_ARR_AT(obj, _vector, n).pdata = NULL;
    break;
  case TYPE_SYMBOL:
    OBJ_ARR_AT(obj, _symbol, n).name = NULL;
    break;
  case TYPE_INT:
  case TYPE_FLOAT:
    break;
  default:
    PL_ASSERT(0, err_typecheck);
  }
  PL_FUNC_END
  return *err;
}

err_t *object_halt(err_t **err, object_t *obj){
  size_t count = object_array_count(err, obj); PL_CHECK;
  while(count --> 0){
    object_halt_nth(err, obj, (int)count);
  }
  PL_FUNC_END
  return *err;
}


// object cast
err_t *object_type_check(err_t **err, object_t *obj, enum_object_type_t type){
  PL_ASSERT(obj!=NULL && obj->type == type, err_typecheck);
  PL_FUNC_END
  return *err;
}

object_float_value_t object_get_value(err_t **err, object_t *obj){
  (void)err;
  switch(obj->type){
    case TYPE_INT:
      return (object_float_value_t)obj->part._int.value;
      break;
    case TYPE_FLOAT:
      return obj->part._float.value;
      break;
    default:
      PL_ASSERT(0, err_typecheck);
  }
  PL_FUNC_END
  return -1;
}



// object tuple
object_t *object_tuple_alloc(err_t **err, gc_manager_t *gcm, size_t size){
  object_t *ret = gc_manager_object_array_alloc(err, gcm, TYPE_REF, size); PL_CHECK;
  PL_FUNC_END
  return ret;
}


// object size
size_t object_sizeof_part(err_t **err, enum_object_type_t obj_type){
  switch(obj_type){
  case TYPE_RAW:       return sizeof(object_raw_part_t);
  case TYPE_INT:       return sizeof(object_int_part_t);
  case TYPE_FLOAT:     return sizeof(object_float_part_t);
  case TYPE_STR:       return sizeof(object_str_part_t);
  case TYPE_SYMBOL:    return sizeof(object_symbol_part_t);
  case TYPE_VECTOR:    return sizeof(object_vector_part_t);
  case TYPE_REF:       return sizeof(object_ref_part_t);
  case TYPE_UNKNOW:    return 0;
  default:             break;
  }
  PL_ASSERT(0, err_typecheck);
  PL_FUNC_END;
  return 0;
}

size_t object_array_sizeof(err_t **err, enum_object_type_t obj_type, size_t n){
  return sizeof(object_header_t) + n * object_sizeof_part(err, obj_type);
}

size_t object_sizeof(err_t **err, enum_object_type_t obj_type){
  return object_array_sizeof(err, obj_type, 1);
}

size_t object_array_count(err_t **err, object_t *obj){
  if(obj == NULL) {return 0;}

  size_t value_size = obj->size - sizeof(object_header_t);
  return value_size==0 ? 0 : value_size / object_sizeof_part(err, obj->type);
}
void* object_array_index(err_t **err, object_t *obj, size_t index){
  PL_ASSERT_NOT_NULL(obj);
  size_t obj_addr = (size_t)obj;
  size_t ret_addr = obj_addr + sizeof(object_header_t) + index * object_sizeof_part(err, obj->type);
  if(ret_addr >= obj_addr + obj->size) {return NULL;}

  PL_FUNC_END_EX(,ret_addr=0);
  return (void*)ret_addr;
}

object_t *object_array_slice(err_t **err, struct gc_manager_t_decl *gcm, object_t *obj, size_t new_count_beg, size_t new_count_end){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  void *src_mem = NULL;
  void *dst_mem = NULL;
  size_t obj_count;
  size_t new_count = new_count_end - new_count_beg;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &obj); PL_CHECK;

  if(new_count == 0){
    ret = gc_manager_object_array_alloc(err, gcm, obj==NULL ? TYPE_UNKNOW : obj->type, 0); PL_CHECK;
  }else{
    PL_ASSERT(new_count_beg <= new_count_end, err_out_of_range);
    obj_count = object_array_count(err, obj); PL_CHECK;
    PL_ASSERT(new_count_end <= obj_count, err_out_of_range);

    ret = gc_manager_object_array_alloc(err, gcm, obj->type, new_count); PL_CHECK;
    src_mem = object_array_index(err, obj, new_count_beg); PL_CHECK;
    dst_mem = object_array_index(err, ret, 0); PL_CHECK;
    memcpy(dst_mem, src_mem, new_count * object_sizeof_part(err, obj->type)); PL_CHECK;
  }
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}

// object vecter
err_t *object_vector_pop(err_t **err, object_t *vec){
  object_type_check(err, vec, TYPE_VECTOR); PL_CHECK;
  if(vec->part._vector.count == 0) {return *err;}
  vec->part._vector.count--;
  object_halt_nth(err, vec->part._vector.pdata, (int)vec->part._vector.count);
  PL_FUNC_END
  return *err;
}

object_t *object_vector_to_array(err_t **err, object_t *vec, struct gc_manager_t_decl *gcm){
  object_type_check(err, vec, TYPE_VECTOR); PL_CHECK;
  return object_array_slice(err, gcm, vec->part._vector.pdata, 0, vec->part._vector.count);
  PL_FUNC_END
  return NULL;
}

err_t *object_vector_index(err_t **err, object_t *vec, int index, object_t *dest){
  // type check
  object_type_check(err, vec, TYPE_VECTOR); PL_CHECK;

  if(index<0) {index = (int)vec->part._vector.count + index;}
  if(dest != NULL){
    object_type_check(err, vec->part._vector.pdata, dest->type); PL_CHECK;
    object_copy_nth(err, vec->part._vector.pdata, index, dest, 0); PL_CHECK;
  }
  PL_FUNC_END
  return *err;
}
err_t *object_vector_top(err_t **err, object_t *vec, object_t *dest){
  return object_vector_index(err, vec, -1, dest);
}
object_t *object_vector_ref_index(err_t **err, object_t *vector, int index){
  object_type_check(err, vector, TYPE_VECTOR); PL_CHECK;
  object_t *ptr = NULL;
  if(index<0) {index = (int)vector->part._vector.count + index;}

  if(vector->part._vector.pdata != NULL){
    // type check
    object_type_check(err, vector->part._vector.pdata, TYPE_REF); PL_CHECK;
    ptr = OBJ_ARR_AT(vector->part._vector.pdata, _ref, index).ptr;
  }

  PL_FUNC_END_EX(, ptr=NULL)
  return ptr;
}

err_t *object_vector_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item){
  size_t gs;
  size_t i;
  size_t max_count;
  object_t *new_data = NULL;

  if(item == NULL) {return *err;}
  gs = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vec); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_data); PL_CHECK;

  object_type_check(err, vec, TYPE_VECTOR); PL_CHECK;

  // able to chage type with size == 0
  if(vec->part._vector.count == 0){
    new_data = gc_manager_object_array_alloc(err, gcm, item->type, 1); PL_CHECK;
    vec->part._vector.pdata = new_data;
  }else{
    // type check
    object_type_check(err, vec->part._vector.pdata, item->type); PL_CHECK;

    max_count = object_array_count(err, vec->part._vector.pdata); PL_CHECK;
    // extend pdata
    if(vec->part._vector.count+1 > max_count){
      new_data =gc_manager_object_array_alloc(err, gcm, item->type, max_count * 2); PL_CHECK;
      for(i=0; i<vec->part._vector.count; i++){
        object_copy_nth(err, vec->part._vector.pdata, (int)i, new_data, (int)i); PL_CHECK;
      }
      vec->part._vector.pdata = new_data;
    }
  }

  object_copy_nth(err, item, 0, vec->part._vector.pdata, (int)vec->part._vector.count); PL_CHECK;
  vec->part._vector.count++;

  PL_FUNC_END_EX(gc_manager_stack_object_balance(gcm,gs),);
  return *err;
}

err_t *object_vector_ref_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item){
  size_t gs;
  object_t *ref_ = NULL;
  gs = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vec); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &ref_); PL_CHECK;

  ref_ = gc_manager_object_alloc_ref(err, gcm, item);
  object_vector_push(err, gcm, vec, ref_); PL_CHECK;

  PL_FUNC_END
  gc_manager_stack_object_balance(gcm,gs);
  return *err;
}

// object str
int object_str_eq(object_t *s1, object_t *s2){
  if(s1==s2) {return 1;}
  if(s1==NULL || s2==NULL) {return 0;}
  if(s1->type != TYPE_STR || s2->type != TYPE_STR) {return 0;}
  if(s1->part._str.size != s2->part._str.size) {return 0;}
  return strncmp(s1->part._str.str, s2->part._str.str, s1->part._str.size) == 0;
}

int object_num_eq(object_t *o1, object_t *o2){
  object_float_value_t v1;
  object_float_value_t v2;
  
  if(o1==o2) {return 1;}
  if(o1==NULL || o2==NULL) {return 0;}
  if(o1->type==TYPE_INT){
    v1 = (object_float_value_t)o1->part._int.value;
  }else if(o1->type==TYPE_FLOAT){
    v1 = (object_float_value_t)o1->part._float.value;
  }else{
    return 0;
  }
  if(o2->type==TYPE_INT){
    v2 = (object_float_value_t)o2->part._int.value;
  }else if(o2->type==TYPE_FLOAT){
    v2 = (object_float_value_t)o2->part._float.value;
  }else{
    return 0;
  }
  
  return ((v1-v2)>-0.0000001) && ((v1-v2)<0.0000001);
}

// verbose
static size_t print_indentation(size_t indentation){
  while(indentation-->0){
    printf(" ");
  }
  return indentation;
}
err_t *object_verbose(err_t **err, object_t *obj, int recursive, size_t indentation, size_t limit){
  size_t count;
  size_t i;

  if(obj==NULL){
    print_indentation(indentation);
    printf("NULL\n");
    return *err;
  }
  count = object_array_count(err, obj); PL_CHECK;
  if(limit!=0 && limit<count) {count=limit;}

  print_indentation(indentation);
  printf("@%p M:"FMT_TYPE_SIZE_T" size: "FMT_TYPE_SIZE_T" ", obj, obj->mark, obj->size);
  switch(obj->type){
    case TYPE_RAW:
      printf("type: raw\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{ptr:%p, autofree:%d} \n", OBJ_ARR_AT(obj, _raw, i).ptr, OBJ_ARR_AT(obj, _raw, i).auto_free);
      }
      break;
    case TYPE_INT:
      printf("type: int\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{d:%ld, x:0x%x} \n", OBJ_ARR_AT(obj, _int, i).value, (unsigned int)OBJ_ARR_AT(obj, _int, i).value);
      }
      break;
    case TYPE_FLOAT:
      printf("type: float\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{lf:%lf} \n", OBJ_ARR_AT(obj, _float, i).value);
      }
      break;
    case TYPE_STR:
      printf("type: str\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{str:%s} \n", OBJ_ARR_AT(obj, _str, i).str);
      }
      break;
    case TYPE_SYMBOL:
      printf("type: symbol\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{symbol:%p} \n", OBJ_ARR_AT(obj, _symbol, i).name);
          object_verbose(err, OBJ_ARR_AT(obj, _symbol, i).name, recursive, indentation+2, 0); PL_CHECK;
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{symbol:%p} \n", OBJ_ARR_AT(obj, _symbol, i).name);
        }
      }
      break;
    case TYPE_VECTOR:
      printf("type: vector\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{count:"FMT_TYPE_SIZE_T"} \n", OBJ_ARR_AT(obj, _vector, i).count);
          if(OBJ_ARR_AT(obj, _vector, i).count > 0){
            object_verbose(err, OBJ_ARR_AT(obj, _vector, i).pdata, recursive-1, indentation+2, OBJ_ARR_AT(obj, _vector, i).count); PL_CHECK;
          }
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{count:"FMT_TYPE_SIZE_T" \n", OBJ_ARR_AT(obj, _vector, i).count);
        }
      }
      break;
    case TYPE_REF:
      printf("type: ref\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{ptr:%p} \n", OBJ_ARR_AT(obj, _ref, i).ptr);
          object_verbose(err, OBJ_ARR_AT(obj, _ref, i).ptr, recursive-1, indentation+2, 0); PL_CHECK;
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{ptr:%p} \n", OBJ_ARR_AT(obj, _ref, i).ptr);
        }
      }
      break;
    default:
      print_indentation(indentation+1);
      printf("type: unknow\n");
  }
  PL_FUNC_END
  return *err;
}

err_t *object_disply(err_t **err, object_t *value){
  size_t count;
  size_t i;
  
  switch(value->type){
    case TYPE_INT:
      printf(FMT_TYPE_INT, value->part._int.value);
      break;
    case TYPE_FLOAT:
      printf(FMT_TYPE_FLOAT, value->part._float.value);
      break;
    case TYPE_STR:
      printf("\"%s\"", value->part._str.str);
      break;
    case TYPE_SYMBOL:
      object_type_check(err, value->part._symbol.name, TYPE_STR); PL_CHECK;
      printf("%s", value->part._symbol.name->part._str.str);
      break;
    case TYPE_REF:
      printf("(");
      count = object_array_count(err, value); PL_CHECK;
      for(i=0; i<count; i++){
        if(i != 0) {printf(" ");}
        object_disply(err, OBJ_ARR_AT(value, _ref, i).ptr); PL_CHECK;
      }
      printf(")");
      break;
    default:
      object_verbose(err, value, 3, 0, 0); PL_CHECK;
  }
  
  PL_FUNC_END
  return *err;
}


// object gc support

err_t *object_mark(err_t **err, object_t *obj, size_t mark, size_t limit){
  size_t count;

  if(limit == 0) {return *err;}
  if(obj==NULL) {return *err;}
  if(obj->mark == mark) {return *err;}

  obj->mark = mark;

  count = object_array_count(err, obj); PL_CHECK
  if(limit<count) {count = limit;}
  if(count == 0) {goto fin;}

  switch(obj->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
    break;
  case TYPE_SYMBOL:
    while(count-->0){
      object_mark(err, OBJ_ARR_AT(obj, _symbol, count).name, mark, UINT_MAX); PL_CHECK;
    }
    break;

  case TYPE_REF:
    while(count-->0){
      object_mark(err, OBJ_ARR_AT(obj, _ref, count).ptr, mark, UINT_MAX); PL_CHECK;
    }
    break;

  case TYPE_VECTOR:
    while(count-->0){
      object_mark(err, OBJ_ARR_AT(obj, _vector, count).pdata, mark, OBJ_ARR_AT(obj, _vector, count).count); PL_CHECK;
    }
    break;

  default:
    PL_ASSERT(0, err_typecheck);
    break;
  }
  PL_FUNC_END
  return *err;
}


err_t *object_move(err_t **err, object_t *obj_old, object_t *obj_new){
  PL_ASSERT_NOT_NULL(obj_old);
  PL_ASSERT_NOT_NULL(obj_new);
  if(obj_old != obj_new){
    memmove(obj_new, obj_old, obj_old->size);
  }
  obj_new->move_dest = NULL;
  PL_FUNC_END
  return *err;
}

err_t *object_ptr_gc_relink(err_t **err, object_t **pobj, char *dest_pool, size_t dest_pool_maxsize){
  PL_ASSERT_NOT_NULL(pobj);
  if(*pobj == NULL) {goto fin;}
  if((*pobj)->move_dest == NULL) {goto fin;}
  PL_ASSERT(dest_pool <= (char*)(*pobj)->move_dest, err_out_of_range);
  PL_ASSERT((char*)(*pobj)->move_dest < dest_pool+dest_pool_maxsize, err_out_of_range);

  *pobj = (*pobj)->move_dest;  // TODO unsure *pobg run this only once
  PL_FUNC_END
  return *err;
}
err_t *object_gc_relink(err_t **err, object_t *obj, char *dest_pool, size_t dest_pool_maxsize){
  size_t count;

  PL_ASSERT_NOT_NULL(obj);
  count = object_array_count(err, obj); PL_CHECK;
  if(count == 0) {goto fin;}

  switch(obj->type){
  case TYPE_SYMBOL:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _symbol, count).name), dest_pool, dest_pool_maxsize); PL_CHECK;
    }
    break;
  case TYPE_REF:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _ref, count).ptr), dest_pool, dest_pool_maxsize); PL_CHECK;
    }
    break;
  case TYPE_VECTOR:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _vector, count).pdata), dest_pool, dest_pool_maxsize); PL_CHECK;
    }
    break;
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  default:
    break;
  }
  PL_FUNC_END
  return *err;
}

err_t *object_ptr_rebase(err_t **err, object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
  PL_ASSERT_NOT_NULL(old_pool);
  PL_ASSERT_NOT_NULL(new_pool);
  size_t old_pool_addr     = (size_t)old_pool;
  size_t old_pool_end_addr = (size_t)old_pool + old_pool_size;
  size_t new_pool_addr     = (size_t)new_pool;
  size_t pobj_addr         = (size_t)*pobj;
  size_t new_pobj_addr;

  if(pobj_addr<old_pool_addr || old_pool_end_addr<=pobj_addr) {return 0;}

  new_pobj_addr = pobj_addr - old_pool_addr + new_pool_addr;

  *pobj = (object_t*)new_pobj_addr;

  PL_FUNC_END
  return *err;
}

err_t *object_rebase(err_t **err, object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
  size_t count;

  PL_ASSERT_NOT_NULL(obj);
  count = object_array_count(err, obj); PL_CHECK;
  if(count == 0) {return *err;}

  switch(obj->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
    break;
  case TYPE_SYMBOL:
    while(count --> 0){
      object_ptr_rebase(err, &(OBJ_ARR_AT(obj, _symbol, count).name), old_pool, old_pool_size, new_pool); PL_CHECK
    }
    break;

  case TYPE_REF:
    while(count --> 0){
      object_ptr_rebase(err, &(OBJ_ARR_AT(obj, _ref, count).ptr), old_pool, old_pool_size, new_pool); PL_CHECK
    }
    break;

  case TYPE_VECTOR:
    while(count --> 0){
      object_ptr_rebase(err, &(OBJ_ARR_AT(obj, _vector, count).pdata), old_pool, old_pool_size, new_pool); PL_CHECK
    }
    break;
  default:
    PL_ASSERT(0, err_typecheck);
    break;
  }

  PL_FUNC_END
  return *err;
}
