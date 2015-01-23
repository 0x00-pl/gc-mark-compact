#include <stdlib.h>
#include <string.h>

#include "pl_gc.h"
#include "pl_type.h"
#include "pl_err.h"



err_t *gc_manager_init(err_t **err, gc_manager_t *manager){
  manager->object_pool = malloc(1024);
  manager->object_pool_maxsize = 1024;
  manager->object_pool_size = 0;
  
  // root object
  gc_manager_object_alloc(err,manager,TYPE_REF); PL_CHECK;
  
  manager->object_pool->mark = 1;
  
  PL_FUNC_END
  return *err;
}

err_t *gc_manager_halt(err_t **err, gc_manager_t *manager){
   
  object_t *iter= NULL;
  
  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err,manager); iter=gc_object_next(iter)){
    object_halt(err, iter); PL_CHECK 
  }
  
  free(manager->object_pool);
  
  PL_FUNC_END
  return *err;
}

object_ref_part_t *gc_manager_root(err_t **err, gc_manager_t *manager){
  return object_as_ref(err, &manager->object_pool[0]);
}


void *mem_pack(void *ptr){
  size_t address= (size_t)ptr;
  address= (address+3) & (~3);
  return (void*)address;
}

object_t *gc_object_next(object_t *obj){
  return (object_t*)mem_pack(((char*)obj) + obj->size);
}
size_t gc_object_offset(object_t *obj_pool, object_t *obj){
  size_t pool_addr = (size_t)obj_pool;
  size_t obj_addr = (size_t)obj;
  return obj_addr - pool_addr;
}
object_t *gc_manager_object_pool_end(err_t **err, gc_manager_t *manager){
  void *ret = mem_pack(((char*)manager->object_pool) + manager->object_pool_size);
  return (object_t*)ret;
}

err_t *gc_manager_object_pool_resize(err_t **err, gc_manager_t *manager, size_t new_size){
   
  object_t *iter;
  object_t *new_object_pool= NULL;
  object_t *old_object_pool= NULL;
  size_t old_object_pool_size= NULL;
  size_t new_maxsize;
  
  old_object_pool = manager->object_pool;
  old_object_pool_size = manager->object_pool_size;
  
  new_maxsize = manager->object_pool_maxsize;
  while(new_maxsize < new_size) { new_maxsize*=2; }
  
  new_object_pool = (object_t*)malloc(new_maxsize);
  PL_ASSERT_NOT_NULL(new_object_pool);
  
  memmove(new_object_pool, manager->object_pool, manager->object_pool_size);
  
  manager->object_pool = new_object_pool;
  manager->object_pool_maxsize = new_maxsize;
  
  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err, manager); iter=gc_object_next(iter)){
    object_rebase(err, iter, old_object_pool, old_object_pool_size, new_object_pool); PL_CHECK 
  }
  
  free(old_object_pool);
  PL_FUNC_END
  return *err;
}

object_t *gc_manager_object_array_alloc(err_t **err, gc_manager_t *manager, enum_object_type_t obj_type, size_t count){
  object_t *new_object= NULL;
  size_t required_size;
  
  required_size = manager->object_pool_size + object_array_sizeof(err, obj_type, count);
  
  if(required_size > manager->object_pool_maxsize){
    if(gc_manager_object_pool_resize(err, manager, required_size)<0) {return NULL;}
  }
  
  new_object = gc_manager_object_pool_end(err, manager);
  new_object->size = object_array_sizeof(err, obj_type, count);
  new_object->type = obj_type;
  
  manager->object_pool_size = gc_object_offset(manager->object_pool, gc_object_next(new_object));

  return new_object;
}
object_t *gc_manager_object_alloc(err_t **err, gc_manager_t *manager, enum_object_type_t obj_type){
  return gc_manager_object_array_alloc(err, manager, obj_type, 1);
}


object_t *gc_manager_object_array_expand(err_t **err, gc_manager_t *gcm, object_t *obj, size_t new_count){
  object_t *ret = NULL;
  void *src_mem = NULL;
  void *dst_mem = NULL;
  size_t new_size = 0;
  
  new_size = object_array_sizeof(err, obj->type, new_count); PL_CHECK;
  PL_ASSERT(obj->size <= new_size, err_out_of_range);
  ret = gc_manager_object_array_alloc(err, gcm, obj->type, new_count); PL_CHECK;
  src_mem = object_array_index(err, obj, 0); PL_CHECK;
  dst_mem = object_array_index(err, ret, 0); PL_CHECK;
  memcpy(dst_mem, src_mem, new_count * object_sizeof_part(err, obj->type)); PL_CHECK;
  PL_FUNC_END
  return ret;
}
object_t *gc_manager_object_array_slice(err_t **err, gc_manager_t *gcm, object_t *obj, size_t new_count_beg, size_t new_count_end){
  object_t *ret = NULL;
  void *src_mem = NULL;
  void *dst_mem = NULL;
  size_t obj_count;
  size_t new_count = new_count_end - new_count_beg;
  size_t new_size = 0;
  
  PL_ASSERT(new_count_beg <= new_count_end, err_out_of_range);
  obj_count = object_array_count(err, obj); PL_CHECK;
  PL_ASSERT(new_count_end <= obj_count, err_out_of_range);
  PL_ASSERT(0 <= new_count_beg, err_out_of_range);
  
  new_size = object_array_sizeof(err, obj->type, new_count); PL_CHECK;
  ret = gc_manager_object_array_alloc(err, gcm, obj->type, new_count); PL_CHECK;
  src_mem = object_array_index(err, obj, new_count_beg); PL_CHECK;
  dst_mem = object_array_index(err, ret, 0); PL_CHECK;
  memcpy(dst_mem, src_mem, new_count * object_sizeof_part(err, obj->type)); PL_CHECK;
  PL_FUNC_END
  return ret;
}



err_t *gc_manager_mark(err_t **err, gc_manager_t *manager){
   
  size_t new_mark = manager->object_pool[0].mark + 1;
  object_mark(err, &manager->object_pool[0], new_mark, UINT_MAX); PL_CHECK 
  PL_FUNC_END
  return *err;
}

static object_t *gc_manager_shadow_object_pool_alloc(err_t **err, gc_manager_t *manager){
  return (object_t*)malloc(manager->object_pool_size);
}

err_t *gc_manager_zip(err_t **err, gc_manager_t *manager){
   
  object_t *new_end;
  object_t *iter = NULL;
  object_t *shadow_object = NULL;
  object_t *shadow_pool = NULL;
  object_gc_broken_part_t *broken_value = NULL;
  size_t object_pool_size = 0;
  size_t mark;
  
  object_pool_size = manager->object_pool_size;
  mark = manager->object_pool[0].mark;
  new_end = manager->object_pool;
  shadow_pool = gc_manager_shadow_object_pool_alloc(err, manager);  PL_CHECK;
  
  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err, manager); iter=gc_object_next(iter)){
    if(iter->mark != mark) {
      object_halt(err, iter); PL_CHECK 
      continue;
    }
    
    
    object_rebase(err, iter, manager->object_pool, object_pool_size, shadow_pool); PL_CHECK 
    
    // fill shadow_pool
    shadow_object = iter;
    object_ptr_rebase(err, &shadow_object, manager->object_pool, object_pool_size, shadow_pool); PL_CHECK 
    
    shadow_object->size = object_sizeof(err, TYPE_GC_BROKEN);
    shadow_object->type = TYPE_GC_BROKEN;
    broken_value = (object_gc_broken_part_t*)object_part(err, shadow_object);  PL_CHECK;
    
    object_move(err, iter, new_end);
    broken_value->ptr= new_end;
    
    new_end = gc_object_next(new_end);
  }
  
  manager->object_pool_size = gc_object_offset(manager->object_pool, new_end);
  
  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err, manager); iter=gc_object_next(iter)){
    object_fix_gc_broken(err, iter);PL_CHECK 
  }
  
  PL_FUNC_END_EX(free(shadow_pool),);
  return *err;
}


err_t *gc_gc(err_t **err, gc_manager_t *manager){
   
  gc_manager_mark(err, manager);  
  gc_manager_zip(err, manager); PL_CHECK 
  PL_FUNC_END
  return *err;
}


err_t *gc_verbose_object_pool(err_t **err, gc_manager_t *manager){
  object_t *iter = NULL;
  size_t count = 0;
  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err, manager); iter=gc_object_next(iter)){
    printf("object%d type: %d, size: %d \n", count, iter->type, iter->size);
    count++;
  }
  return *err;
}


