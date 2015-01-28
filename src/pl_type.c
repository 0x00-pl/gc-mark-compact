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
  case TYPE_GC_BROKEN : return "TYPE_GC_BROKEN";
  case TYPE_VECTOR    : return "TYPE_VECTOR";
  case TYPE_REF       : return "TYPE_REF";
  default             : return "TYPE_UNKNOW";
  }
  return "TYPE_UNKNOW";
}

// object init/halt
#define TMP_OBJECT_INIT(_tname, tname_enum, args, body) \
err_t *object##_tname##_init args{ \
  object##_tname##_part_t *_part = NULL; \
  PL_ASSERT_NOT_NULL(thiz); \
  _part = object_as##_tname (err, thiz); PL_CHECK; \
  body; PL_CHECK; \
  PL_FUNC_END \
  return *err; \
}

TMP_OBJECT_INIT(_raw, TYPE_RAW, (err_t **err, object_t *thiz, void *ptr, int auto_free),
                object_raw_part_init(err, _part, ptr, auto_free);
               )

TMP_OBJECT_INIT(_int, TYPE_INT, (err_t **err, object_t *thiz, long int value),
                object_int_part_init(err, _part, value);
               )

TMP_OBJECT_INIT(_float, TYPE_FLOAT, (err_t **err, object_t *thiz, double value),
                object_float_part_init(err, _part, value);
               )

TMP_OBJECT_INIT(_str, TYPE_STR, (err_t **err, object_t *thiz, const char* text),
                object_str_part_init(err, _part, text);
               )

TMP_OBJECT_INIT(_symbol, TYPE_SYMBOL, (err_t **err, object_t *thiz, object_t *name),
                object_symbol_part_init(err, _part, name);
               )

TMP_OBJECT_INIT(_gc_broken, TYPE_GC_BROKEN, (err_t **err, object_t *thiz, object_t *ptr),
                object_gc_broken_part_init(err, _part, ptr);
               )

TMP_OBJECT_INIT(_vector, TYPE_VECTOR, (err_t **err, object_t *thiz),
                object_vector_part_init(err, _part);
               )

TMP_OBJECT_INIT(_ref, TYPE_REF, (err_t **err, object_t *thiz, object_t *ptr),
                object_ref_part_init(err, _part, ptr);
               )

#undef TMP_OBJECT_INIT

err_t *object_raw_halt(err_t **err, object_t *thiz){
  object_raw_part_t *_part = object_as_raw(err, thiz); PL_CHECK;
  if(_part->auto_free && _part->ptr!=NULL){
    free(_part->ptr);
  }
  PL_FUNC_END
  return *err;
}

err_t *object_str_halt(err_t **err, object_t *thiz){
  object_str_part_t *_part = object_as_str(err, thiz); PL_CHECK;
  if(_part->str!=NULL){
    free(_part->str);
  }
  PL_FUNC_END
  return *err;
}

err_t *object_halt(err_t **err, object_t *obj){
  PL_ASSERT_NOT_NULL(obj);
  switch(obj->type){
  case TYPE_RAW:
    object_raw_halt(err, obj); PL_CHECK;
    break;
  case TYPE_STR:
    object_str_halt(err, obj); PL_CHECK;
    break;
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_SYMBOL:
  case TYPE_GC_BROKEN:
  case TYPE_REF:
  case TYPE_VECTOR:
  case TYPE_UNKNOW:
    break;
  default:
    PL_ASSERT(0, err_typecheck);
    break;
  }
  PL_FUNC_END
  return *err;
}

err_t *object_raw_part_init(err_t **err, object_raw_part_t *part, void *ptr, int auto_free){
  part->ptr = ptr;
  part->auto_free = auto_free;
  return *err;
}
err_t *object_int_part_init(err_t **err, object_int_part_t *part, long int value){
  part->value = value;
  return *err;
}
err_t *object_float_part_init(err_t **err, object_float_part_t *part, double value){
  part->value = value;
  return *err;
}
err_t *object_str_part_init(err_t **err, object_str_part_t *part, const char* text){
  char *text_copy = NULL;
  part->size = strlen(text);
  text_copy = malloc(part->size+1);
  strncpy(text_copy, text, part->size);
  text_copy[part->size] = '\0';
  part->str = text_copy;
  return *err;
}
err_t *object_symbol_part_init(err_t **err, object_symbol_part_t *part, object_t *name){
  part->name = name;
  return *err;
}
err_t *object_gc_broken_part_init(err_t **err, object_gc_broken_part_t *part, object_t *ptr){
  part->ptr = ptr;
  return *err;
}
err_t *object_vector_part_init(err_t **err, object_vector_part_t *part){
  part->count = 0;
  part->pdata = NULL;
  return *err;
}
err_t *object_ref_part_init(err_t **err, object_ref_part_t *part, object_t *ptr){
  part->ptr = ptr;
  return *err;
}
err_t *object_part_halt(err_t **err, void *part, enum_object_type_t type){
  object_raw_part_t       *raw_part       = (object_raw_part_t       *)part;
//   object_int_part_t       *int_part       = (object_int_part_t       *)part;
//   object_float_part_t     *float_part     = (object_float_part_t     *)part;
  object_str_part_t       *str_part       = (object_str_part_t       *)part;
  object_symbol_part_t    *symbol_part    = (object_symbol_part_t    *)part;
  object_gc_broken_part_t *gc_broken_part = (object_gc_broken_part_t *)part;
  object_vector_part_t    *vector_part    = (object_vector_part_t    *)part;
  object_ref_part_t       *ref_part       = (object_ref_part_t       *)part;
  
  switch(type){
  case TYPE_RAW:
    if(raw_part->auto_free){
      free(raw_part->ptr);
    }
    break;
  case TYPE_STR:
    free(str_part->str);
    break;
  case TYPE_REF:
    ref_part->ptr = NULL;
    break;
  case TYPE_VECTOR:
    vector_part->count = 0;
    vector_part->pdata = NULL;
    break;
  case TYPE_SYMBOL:
    symbol_part->name = NULL;
    break;
  case TYPE_GC_BROKEN:
    gc_broken_part->ptr = NULL;
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


// object copy init

err_t *object_copy_init(err_t **err, object_t *src, object_t *dst){
  PL_ASSERT_NOT_NULL(src);
  PL_ASSERT_NOT_NULL(dst);
  
  switch(src->type){
  case TYPE_RAW:
  case TYPE_INT:
  case TYPE_FLOAT:
  case TYPE_STR:
  case TYPE_SYMBOL:
  case TYPE_GC_BROKEN:
  case TYPE_REF:
  case TYPE_VECTOR:
    memcpy(dst, src, src->size);
    break;
  default:
    PL_ASSERT(0, err_typecheck);
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
void* object_part(err_t **err, object_t *obj){
  (void)err;
  if(obj==NULL) {return NULL;}
//   size_t obj_addr = (size_t)obj;
//   return (void*)(obj_addr + sizeof(object_header_t));
  return &(obj->part._unknow);
}

#define TMP_OBJECT_AS_DECL(_tname, tname_enum) \
object##_tname##_part_t *object_as##_tname (err_t **err, object_t *obj){ \
  PL_ASSERT_NOT_NULL(obj); \
  object_type_check(err, obj, tname_enum); PL_CHECK; \
  PL_FUNC_END_EX(, return NULL); \
  return (object##_tname##_part_t*)object_part(err, obj); \
}

TMP_OBJECT_AS_DECL(_raw,       TYPE_RAW      )
TMP_OBJECT_AS_DECL(_int,       TYPE_INT      )
TMP_OBJECT_AS_DECL(_float,     TYPE_FLOAT    )
TMP_OBJECT_AS_DECL(_str,       TYPE_STR      )
TMP_OBJECT_AS_DECL(_symbol,    TYPE_SYMBOL   )
TMP_OBJECT_AS_DECL(_gc_broken, TYPE_GC_BROKEN)
TMP_OBJECT_AS_DECL(_vector,    TYPE_VECTOR   )
TMP_OBJECT_AS_DECL(_ref,       TYPE_REF      )

#undef TMP_OBJECT_AS_DECL


// object tuple
object_t *object_tuple_alloc(err_t **err, gc_manager_t *gcm, size_t size){
  object_t *ret = gc_manager_object_array_alloc(err, gcm, TYPE_REF, size); PL_CHECK;
  PL_FUNC_END
  return ret;
}
err_t *object_member_set_value(err_t **err, object_t *tuple, size_t offset, object_t *value){
  PL_ASSERT(offset<object_array_count(err, tuple), err_out_of_range);
  PL_ASSERT_NOT_NULL(object_as_ref(err, tuple));
  object_as_ref(err, tuple)[offset].ptr = value;
  PL_FUNC_END
  return *err;
}
void *object_member(err_t **err, object_t *tuple, size_t offset){
  object_as_ref(err, tuple); PL_CHECK;
  PL_ASSERT(offset<object_array_count(err, tuple), err_out_of_range);
  PL_ASSERT_NOT_NULL(object_as_ref(err, tuple)[offset].ptr);
  PL_FUNC_END_EX(,return NULL);
  return object_part(err, object_as_ref(err, tuple)[offset].ptr);
}

#define TMP_OBJECT_MEMBER_DECL(_tname, tname_enum) \
object##_tname##_part_t *object_member##_tname (err_t **err, object_t *tuple, size_t offset){ \
  object_as_ref(err, tuple); PL_CHECK; \
  object_type_check(err, object_as_ref(err, tuple)[offset].ptr, tname_enum); PL_CHECK; \
  PL_FUNC_END_EX(,return NULL); \
  return (object##_tname##_part_t*)object_member(err, tuple, offset); \
}

TMP_OBJECT_MEMBER_DECL(_raw,       TYPE_RAW      )
TMP_OBJECT_MEMBER_DECL(_int,       TYPE_INT      )
TMP_OBJECT_MEMBER_DECL(_float,     TYPE_FLOAT    )
TMP_OBJECT_MEMBER_DECL(_str,       TYPE_STR      )
TMP_OBJECT_MEMBER_DECL(_symbol,    TYPE_SYMBOL   )
TMP_OBJECT_MEMBER_DECL(_gc_broken, TYPE_GC_BROKEN)
TMP_OBJECT_MEMBER_DECL(_vector,    TYPE_VECTOR   )
TMP_OBJECT_MEMBER_DECL(_ref,       TYPE_REF      )

#undef TMP_OBJECT_MEMBER_DECL

// object size

size_t object_sizeof_part(err_t **err, enum_object_type_t obj_type){
  switch(obj_type){
  case TYPE_RAW:       return sizeof(object_raw_part_t);
  case TYPE_INT:       return sizeof(object_int_part_t);
  case TYPE_FLOAT:     return sizeof(object_float_part_t);
  case TYPE_STR:       return sizeof(object_str_part_t);
  case TYPE_SYMBOL:    return sizeof(object_symbol_part_t);
  case TYPE_GC_BROKEN: return sizeof(object_gc_broken_part_t);
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



// object vecter
err_t *object_vector_part_pop(err_t **err, object_vector_part_t *vector_part, object_t *dest){
  void *src_part = NULL;
  void *dst_part = NULL;
  
  if(vector_part->count == 0) {return *err;}
  vector_part->count--;  

  src_part = object_array_index(err, vector_part->pdata, vector_part->count); PL_CHECK;
  if(dest!=NULL){
    // type check
    PL_ASSERT(vector_part->pdata->type == dest->type, err_typecheck);
    dst_part = object_part(err, dest); PL_CHECK;
    object_part_move(err, src_part, dst_part, vector_part->pdata->type); PL_CHECK;
  }
  
  object_part_halt(err, src_part, vector_part->pdata->type); PL_CHECK;
  
  if(vector_part->count == 0){
    vector_part->pdata = NULL;
  }
  PL_FUNC_END
  return *err;
}

void *object_vector_part_top(err_t **err, object_vector_part_t *vector_part, object_t *dest){
  void *src_part = NULL;
  void *dst_part = NULL;
  
  // type check
  PL_ASSERT(vector_part->pdata->type == dest->type, err_typecheck); 

  src_part = object_array_index(err, vector_part->pdata, vector_part->count-1); PL_CHECK;
  if(dest!=NULL){
    dst_part = object_part(err, dest); PL_CHECK;
    object_part_move(err, src_part, dst_part, vector_part->pdata->type); PL_CHECK;
  }
 
  PL_FUNC_END
  return src_part;
}

object_t *object_vector_part_to_array(err_t **err, object_vector_part_t *vector_part, struct gc_manager_t_decl *gcm){
  object_t *new_array = NULL;
  new_array = gc_manager_object_array_slice(err, gcm, vector_part->pdata, 0, vector_part->count); PL_CHECK;
  PL_FUNC_END
  return new_array;
}

void *object_vector_part_index(err_t **err, object_vector_part_t *vector_part, int index, object_t *dest){
  void *src_part = NULL;
  void *dst_part = NULL;
  

  if(index<0) {index = (int)vector_part->count + index;}
  
  src_part = object_array_index(err, vector_part->pdata, (size_t)index); PL_CHECK;
  if(dest!=NULL){
    // type check
    PL_ASSERT(vector_part->pdata->type == dest->type, err_typecheck);
    
    dst_part = object_part(err, dest); PL_CHECK;
    object_part_move(err, src_part, dst_part, vector_part->pdata->type); PL_CHECK;
  }
 
  PL_FUNC_END
  return src_part;
}
static object_t *object_vector_part_ref_index(err_t **err, object_vector_part_t *vector_part, int index){
  object_t *ptr = NULL;
  if(index<0) {index = (int)vector_part->count + index;}
  
  if(vector_part->pdata != NULL){
    // type check
    object_type_check(err, vector_part->pdata, TYPE_REF); PL_CHECK;
    ptr = OBJ_ARR_AT(vector_part->pdata, _ref, index).ptr;
  }
  
  PL_FUNC_END_EX(, ptr=NULL)
  return ptr;
}
enum_object_type_t object_vector_part_type(err_t **err, object_vector_part_t *vector_part){
  (void)err;
  if(vector_part->pdata == NULL){
    return TYPE_UNKNOW;
  }
  return vector_part->pdata->type;
}

err_t *object_vector_push(err_t **err, struct gc_manager_t_decl *gcm, object_t *vec, object_t *item){
  size_t gs;
  size_t max_count;
  void *src;
  void *dst;
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
      new_data = gc_manager_object_array_expand(err, gcm, vec->part._vector.pdata, max_count * 2); PL_CHECK;
      vec->part._vector.pdata = new_data;
    }
  }
  src = object_array_index(err, item, 0); PL_CHECK;
  dst = object_array_index(err, vec->part._vector.pdata, vec->part._vector.count); PL_CHECK;
  object_part_move(err, src, dst, vec->part._vector.pdata->type); PL_CHECK;
  
  vec->part._vector.count++;
  
  PL_FUNC_END_EX(gc_manager_stack_object_balance(gcm,gs),);
  return *err;
}
err_t *object_vector_pop(err_t **err, object_t *vec, object_t *dest){
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  object_vector_part_pop(err, vector_part, dest); PL_CHECK;
  PL_FUNC_END
  return *err;
}
void *object_vector_top(err_t **err, object_t *vec, object_t *dest){
  void *ret = NULL;
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  ret = object_vector_part_top(err, vector_part, dest); PL_CHECK;
  PL_FUNC_END
  return ret;
}

object_t *object_vector_to_array(err_t **err, object_t *vec, struct gc_manager_t_decl *gcm){
  object_t *ret = NULL;
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  ret = object_vector_part_to_array(err, vector_part, gcm); PL_CHECK;
  PL_FUNC_END
  return ret;
}
void *object_vector_index(err_t **err, object_t *vec, int index, object_t *dest){
  void *ret = NULL;
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  ret = object_vector_part_index(err, vector_part, index, dest); PL_CHECK;
  PL_FUNC_END
  return ret;
}
object_t *object_vector_ref_index(err_t **err, object_t *vec, int index){
  void *ret = NULL;
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  ret = object_vector_part_ref_index(err, vector_part, index); PL_CHECK;
  PL_FUNC_END
  return ret;
}
enum_object_type_t object_vector_type(err_t **err, object_t *vec){
  enum_object_type_t ret;
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  ret = object_vector_part_type(err, vector_part); PL_CHECK;
  PL_FUNC_END
  return ret;
}
size_t object_vector_count(err_t **err, object_t *vec){
  object_vector_part_t *vector_part = object_as_vector(err, vec); PL_CHECK;
  PL_FUNC_END
  return vector_part->count;
}

// object str
int object_str_eq(object_t *s1, object_t *s2){
  if(s1==s2) {return 1;}
  if(s1==NULL || s2==NULL) {return 0;}
  if(s1->type != TYPE_STR || s2->type != TYPE_STR) {return 0;}
  if(s1->part._str.size != s2->part._str.size) {return 0;}
  return strncmp(s1->part._str.str, s2->part._str.str, s1->part._str.size) == 0;
}

// verbose
static size_t print_indentation(size_t indentation){
  while(indentation-->0){
    printf(" ");
  }
  return indentation;
}
err_t *object_verbose(err_t **err, object_t *obj, int recursive, size_t indentation, size_t limit){
  object_raw_part_t       *raw_part;
  object_int_part_t       *int_part;
  object_float_part_t     *float_part;
  object_str_part_t       *str_part;
  object_symbol_part_t    *symbol_part;
  object_gc_broken_part_t *gc_broken_part;
  object_vector_part_t    *vector_part;
  object_ref_part_t       *ref_part;
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
  printf("@%p size: %zu ", obj, obj->size);
  switch(obj->type){
    case TYPE_RAW:
      raw_part = object_as_raw(err, obj); PL_CHECK;
      printf("type: raw\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{ptr:%p, autofree:%d} \n", raw_part[i].ptr, raw_part[i].auto_free);
      }
      break;
    case TYPE_INT:
      int_part = object_as_int(err, obj); PL_CHECK;
      printf("type: int\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{d:%ld, x:0x%x} \n", int_part[i].value, (unsigned int)int_part[i].value);
      }
      break;
    case TYPE_FLOAT:
      float_part = object_as_float(err, obj); PL_CHECK;
      printf("type: float\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{lf:%lf} \n", float_part[i].value);
      }
      break;
    case TYPE_STR:
      str_part = object_as_str(err, obj); PL_CHECK;
      printf("type: str\n");
      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{str:%s} \n", str_part[i].str);
      }
      break;
    case TYPE_SYMBOL:
      symbol_part = object_as_symbol(err, obj); PL_CHECK;
      printf("type: symbol\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{symbol:%p} \n", symbol_part[i].name);
          object_verbose(err, symbol_part[i].name, recursive-1, indentation+2, 0); PL_CHECK;
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{symbol:%p} \n", symbol_part[i].name);
        }
      }
      break;
    case TYPE_GC_BROKEN:
      gc_broken_part = object_as_gc_broken(err, obj); PL_CHECK;
      printf("type: gc-broken\n");

      for(i=0; i<count; i++){
        print_indentation(indentation+1);
        printf("{gc-broken:%p} \n", gc_broken_part[i].ptr);
      }
      break;
    case TYPE_VECTOR:
      vector_part = object_as_vector(err, obj); PL_CHECK;
      printf("type: vector\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{count:%zu} \n", vector_part[i].count);
          if(vector_part[i].count > 0){
            object_verbose(err, vector_part[i].pdata, recursive-1, indentation+2, vector_part[i].count); PL_CHECK;
          }
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{count:%zu} \n", vector_part[i].count);
        }
      }
      break;
    case TYPE_REF:
      ref_part = object_as_ref(err, obj); PL_CHECK;
      printf("type: ref\n");
      if(recursive>0){
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{ptr:%p} \n", ref_part[i].ptr);
          object_verbose(err, ref_part[i].ptr, recursive-1, indentation+2, 0); PL_CHECK;
        }
      }else{
        for(i=0; i<count; i++){
          print_indentation(indentation+1);
          printf("{ptr:%p} \n", ref_part[i].ptr);
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


// object gc support

err_t *object_mark(err_t **err, object_t *obj, size_t mark, size_t limit){
  object_symbol_part_t *symbol_part = NULL;
  object_vector_part_t *vector_part = NULL;
  object_ref_part_t *ref_part = NULL;
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
    symbol_part = object_as_symbol(err, obj); PL_CHECK;
    while(count-->0){
      object_mark(err, symbol_part[count].name, mark, UINT_MAX); PL_CHECK;
    }
    break;
    
  case TYPE_REF:
    ref_part = object_as_ref(err, obj); PL_CHECK;
    while(count-->0){
      object_mark(err, ref_part[count].ptr, mark, UINT_MAX); PL_CHECK;
    }
    break;
    
  case TYPE_VECTOR:
    vector_part = object_as_vector(err, obj); PL_CHECK;
    while(count-->0){
      object_mark(err, vector_part[count].pdata, mark, vector_part[count].count); PL_CHECK;
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
err_t *object_part_move(err_t **err, void *part_src, void *part_dst, enum_object_type_t type){
  size_t copy_size = object_sizeof_part(err, type); PL_CHECK;
  PL_ASSERT_NOT_NULL(part_src);
  PL_ASSERT_NOT_NULL(part_dst);
  if(part_src != part_dst){
    memmove(part_dst, part_src, copy_size);
  }
  PL_FUNC_END
  return *err;
}

err_t *object_ptr_gc_relink(err_t **err, object_t **pobj){
  PL_ASSERT_NOT_NULL(pobj);
  if(*pobj == NULL) {goto fin;}
  *pobj = (*pobj)->move_dest;
  PL_FUNC_END
  return *err;
}
err_t *object_gc_relink(err_t **err, object_t *obj){
  size_t count;
  
  PL_ASSERT_NOT_NULL(obj);
  count = object_array_count(err, obj); PL_CHECK;
  if(count == 0) {goto fin;}

  switch(obj->type){
  case TYPE_SYMBOL:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _symbol, count).name)); PL_CHECK;
    }
    break;
  case TYPE_REF:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _ref, count).ptr)); PL_CHECK;
    }
    break;
  case TYPE_VECTOR:
    while(count-->0){
      object_ptr_gc_relink(err, &(OBJ_ARR_AT(obj, _vector, count).pdata)); PL_CHECK;
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

// err_t *obj_ptr_fix_gc_broken(err_t **err, object_t **pobj){
//   PL_ASSERT_NOT_NULL(pobj);
//   if(*pobj == NULL) {return *err;}
//   
//   object_gc_broken_part_t *broken_value = NULL;
//   
//   if((*pobj)->type == TYPE_GC_BROKEN){
//     broken_value = object_as_gc_broken(err, *pobj);
//     PL_ASSERT_NOT_NULL(broken_value);
//     *pobj = broken_value->ptr;
//   }
//   PL_FUNC_END
//   return *err;
// }
// err_t *object_fix_gc_broken(err_t **err, object_t *obj){
//   object_symbol_part_t *symbol_part = NULL;
//   object_vector_part_t *vector_part = NULL;
//   object_ref_part_t *ref_part = NULL;
//   size_t count;
//   
//   PL_ASSERT_NOT_NULL(obj);
//   count = object_array_count(err, obj); PL_CHECK;
//   if(count == 0) {return *err;}
// 
//   switch(obj->type){
//   case TYPE_SYMBOL:
//     symbol_part = object_as_symbol(err, obj); PL_CHECK;
//     while(count-->0){
//       obj_ptr_fix_gc_broken(err, &(symbol_part[count].name)); PL_CHECK;
//     }
//     break;
//   case TYPE_REF:    
//     ref_part = object_as_ref(err, obj); PL_CHECK;
//     while(count-->0){
//       obj_ptr_fix_gc_broken(err, &(ref_part[count].ptr)); PL_CHECK;
//     }
//     break;
//   case TYPE_VECTOR:
//     vector_part = object_as_vector(err, obj); PL_CHECK;
//     while(count-->0){
//       obj_ptr_fix_gc_broken(err, &(vector_part[count].pdata)); PL_CHECK;
//     }
//     break;
//   case TYPE_RAW:
//   case TYPE_INT:
//   case TYPE_FLOAT:
//   case TYPE_STR:
//   default:
//     break;
//   }
//   PL_FUNC_END
//   return *err;
// }


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
  
  
  object_symbol_part_t *symbol_part = NULL;
  object_vector_part_t *vector_part = NULL;
  object_ref_part_t *ref_part = NULL;
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
    symbol_part = object_as_symbol(err, obj); PL_CHECK;
    while(count --> 0){
      object_ptr_rebase(err, &(symbol_part[count].name), old_pool, old_pool_size, new_pool); PL_CHECK 
    }
    break;
    
  case TYPE_REF:
    ref_part = object_as_ref(err, obj); PL_CHECK;
    while(count --> 0){
      object_ptr_rebase(err, &(ref_part[count].ptr), old_pool, old_pool_size, new_pool); PL_CHECK 
    }
    break;
    
  case TYPE_VECTOR:
    vector_part = object_as_vector(err, obj); PL_CHECK;
    while(count --> 0){
      object_ptr_rebase(err, &(vector_part[count].pdata), old_pool, old_pool_size, new_pool); PL_CHECK 
    }
    break;
  default:
    PL_ASSERT(0, err_typecheck);
    break;
  }
  
  PL_FUNC_END
  return *err;
}
