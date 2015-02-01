#include <stdlib.h>
#include <string.h>

#include "pl_gc.h"
#include "pl_type.h"
#include "pl_err.h"
#include "pl_op_code.h"
#include "pl_plandform.h"


err_t *gc_manager_init(err_t **err, gc_manager_t *manager){

  manager->stack_pobject_pool = (object_t***)malloc(10 * sizeof(object_t**));
  manager->stack_pobject_pool_maxcount = 10;
  manager->stack_pobject_pool_count = 0;

  manager->object_pool = malloc(1);
  manager->object_pool_maxsize = 1;
  manager->object_pool_size = 0;

  manager->object_count = 0;

  manager->cur_mark = 0;

  return *err;
}

err_t *gc_manager_halt(err_t **err, gc_manager_t *manager){

  object_t *iter= NULL;

  for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err,manager); iter=gc_object_next(iter)){
    object_halt(err, iter); PL_CHECK
  }

  free(manager->object_pool);
  free(manager->stack_pobject_pool);
  free(manager);

  PL_FUNC_END
  return *err;
}

void *mem_pack(void *ptr){
  size_t address= (size_t)ptr;
  address= (address+3) & (size_t)(~3);
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
  (void)err;
  void *ret = mem_pack(((char*)manager->object_pool) + manager->object_pool_size);
  return (object_t*)ret;
}

err_t *gc_manager_stack_object_push(err_t **err, gc_manager_t *manager, object_t** pobj){
  object_t ***new_pobject_pool = NULL;

  if(manager->stack_pobject_pool_count+1 >= manager->stack_pobject_pool_maxcount){
    // expand
    manager->stack_pobject_pool_maxcount *= 2;
    new_pobject_pool = (object_t ***)malloc(manager->stack_pobject_pool_maxcount * sizeof(object_t**));
    memcpy(new_pobject_pool, manager->stack_pobject_pool, manager->stack_pobject_pool_count * sizeof(object_t**));

    free(manager->stack_pobject_pool);

    manager->stack_pobject_pool = new_pobject_pool;
  }


  manager->stack_pobject_pool[manager->stack_pobject_pool_count] = pobj;
  manager->stack_pobject_pool_count++;

  return *err;
}
err_t *gc_manager_stack_object_pop(err_t **err, gc_manager_t *manager, size_t stack_ptr_count){
  PL_ASSERT(manager->stack_pobject_pool_count > stack_ptr_count, err_out_of_range);
  manager->stack_pobject_pool_count -= stack_ptr_count;
  PL_FUNC_END
  return *err;
}
size_t gc_manager_stack_object_get_depth(gc_manager_t *manager){
  return manager->stack_pobject_pool_count;
}
int gc_manager_stack_object_balance(gc_manager_t *manager, size_t depth){
  manager->stack_pobject_pool_count = depth;
  return 0;
}
err_t *gc_manager_object_pool_resize(err_t **err, gc_manager_t *manager, size_t new_size){
  size_t new_maxsize;
  object_t *old_object_pool= NULL;
  object_t *new_object_pool= NULL;

  new_maxsize = manager->object_pool_maxsize;
  while(new_maxsize < new_size){
 // printf("[debug] change "FMT_SIXE_T" / "FMT_SIXE_T" => "FMT_SIXE_T"\n",
 //        manager->object_pool_size, manager->object_pool_maxsize, new_size);
    new_maxsize *= 2;
  }

  old_object_pool = manager->object_pool;
  new_object_pool = (object_t*)malloc(new_maxsize);

  gc_manager_mark(err, manager); PL_CHECK;
  gc_manager_compact(err, manager, new_object_pool, new_maxsize); PL_CHECK;

  PL_FUNC_END

  free(old_object_pool);

  return *err;
}

object_t *gc_manager_object_array_alloc(err_t **err, gc_manager_t *manager, enum_object_type_t obj_type, size_t count){
  object_t *new_object= NULL;
  size_t required_size;

  required_size = manager->object_pool_size + object_array_sizeof(err, obj_type, count);

  if(required_size > manager->object_pool_maxsize){
    gc_manager_object_pool_resize(err, manager, required_size); PL_CHECK;
  }

  new_object = gc_manager_object_pool_end(err, manager);
  new_object->size = object_array_sizeof(err, obj_type, count);
  new_object->type = obj_type;
  new_object->mark = 0;
  new_object->move_dest = NULL;

  manager->object_pool_size = gc_object_offset(manager->object_pool, gc_object_next(new_object));
  manager->object_count++;


  while(count-->0){
    object_init_nth(err, new_object, (int)count); PL_CHECK;
  }

  PL_FUNC_END_EX(,new_object=NULL);
  return new_object;
}
object_t *gc_manager_object_alloc(err_t **err, gc_manager_t *manager, enum_object_type_t obj_type){
  return gc_manager_object_array_alloc(err, manager, obj_type, 1);
}
object_t *gc_manager_object_alloc_ref(err_t **err, gc_manager_t *manager, object_t *obj){
  object_t *ref_ = NULL;

  gc_manager_stack_object_push(err, manager, &obj); PL_CHECK;

  ref_ = gc_manager_object_alloc(err, manager, TYPE_REF); PL_CHECK;
  object_ref_init(err, ref_, obj); PL_CHECK;

  PL_FUNC_END_EX(
    gc_manager_stack_object_pop(err, manager, 1);
    , ref_=NULL);

  return ref_;
}
err_t *gc_manager_object_reserve(err_t **err, gc_manager_t *manager, size_t mem_size){
  size_t required_size;

  required_size = manager->object_pool_size + mem_size;

  if(required_size > manager->object_pool_maxsize){
    gc_manager_object_pool_resize(err, manager, required_size); PL_CHECK;
  }

  PL_FUNC_END
  return *err;
}


object_t *gc_manager_object_array_expand(err_t **err, gc_manager_t *gcm, object_t *obj, size_t new_count){
  object_t *ret = NULL;
  void *src_mem = NULL;
  void *dst_mem = NULL;
  size_t new_size = 0;
  size_t old_count;

  new_size = object_array_sizeof(err, obj->type, new_count); PL_CHECK;
  PL_ASSERT(obj->size <= new_size, err_out_of_range);

  old_count = object_array_count(err, obj); PL_CHECK;
  ret = gc_manager_object_array_alloc(err, gcm, obj->type, new_count); PL_CHECK;
  src_mem = object_array_index(err, obj, 0); PL_CHECK;
  dst_mem = object_array_index(err, ret, 0); PL_CHECK;
  memset(dst_mem, 0, new_count * object_sizeof_part(err, obj->type));
  memcpy(dst_mem, src_mem, old_count * object_sizeof_part(err, obj->type)); PL_CHECK;
  PL_FUNC_END
  return ret;
}
object_t *gc_manager_object_array_slice(err_t **err, gc_manager_t *gcm, object_t *obj, size_t new_count_beg, size_t new_count_end){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  void *src_mem = NULL;
  void *dst_mem = NULL;
  size_t obj_count;
  size_t new_count = new_count_end - new_count_beg;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &obj); PL_CHECK;

  if(new_count == 0){
    ret = gc_manager_object_array_alloc(err, gcm, TYPE_UNKNOW, 0); PL_CHECK;
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



err_t *gc_manager_mark(err_t **err, gc_manager_t *manager){
  size_t i;
  manager->cur_mark++;
  //object_mark(err, &manager->object_pool[0], new_mark, UINT_MAX); PL_CHECK
  for(i=0; i<manager->stack_pobject_pool_count; i++){
    object_mark(err, *manager->stack_pobject_pool[i], manager->cur_mark, UINT_MAX); PL_CHECK;
  }
  PL_FUNC_END
  return *err;
}

err_t *gc_manager_compact(err_t **err, gc_manager_t *manager, object_t *compact_dest, size_t dest_pool_size){
  size_t i;
  object_t *new_end;
  object_t *iter = NULL;
  object_t *move_dest = NULL;
  object_t *iter_end = NULL;
  size_t cur_mark;

  cur_mark = manager->cur_mark;

  if(compact_dest==NULL) {
    compact_dest =  manager->object_pool;
    dest_pool_size = manager->object_pool_maxsize;
  }
  new_end = compact_dest;

  iter_end = gc_manager_object_pool_end(err, manager); PL_CHECK;
  // fill move_dest
  for(iter=manager->object_pool; iter<iter_end; iter=gc_object_next(iter)){
    if(iter->mark != cur_mark){
      iter->move_dest = NULL;
      continue;
    }
    iter->move_dest = new_end;
    new_end = mem_pack(((char*)new_end) + iter->size);
  }
  // relink ptr
  for(iter=manager->object_pool; iter<iter_end; iter=gc_object_next(iter)){
    if(iter->mark != cur_mark) {
      continue;
    }
    object_gc_relink(err, iter, (char*)compact_dest, dest_pool_size); PL_CHECK;
  }
  // relink stack ptr
  for(i=0; i<manager->stack_pobject_pool_count; i++){
    object_ptr_gc_relink(err, manager->stack_pobject_pool[i], (char*)compact_dest, dest_pool_size); PL_CHECK;
  }
  // move object
  for(iter=manager->object_pool; iter<iter_end; iter=gc_object_next(iter)){
    if(iter->mark != cur_mark) {
      manager->object_count--;
      object_halt(err, iter); PL_CHECK;
      continue;
    }
    move_dest = iter->move_dest;
    PL_ASSERT((char*)compact_dest <= (char*)move_dest, err_out_of_range);
    PL_ASSERT((char*)move_dest < (char*)compact_dest+dest_pool_size, err_out_of_range);
    object_move(err, iter, move_dest); PL_CHECK;
  }


  manager->object_pool = compact_dest;
  manager->object_pool_maxsize = dest_pool_size;
  manager->object_pool_size = gc_object_offset(compact_dest, new_end);

  PL_ASSERT(manager->object_pool_size<=manager->object_pool_maxsize, err_out_of_range);

  PL_FUNC_END
  return *err;
}

err_t *gc_gc(err_t **err, gc_manager_t *manager){
  //gc_manager_mark(err, manager); //old code
  //gc_manager_compact(err, manager, NULL, 0); PL_CHECK //old code
  gc_manager_object_pool_resize(err, manager, manager->object_pool_maxsize); PL_CHECK;
  PL_FUNC_END
  return *err;
}


err_t *gc_verbose_object_pool(err_t **err, gc_manager_t *manager, int detal){
  size_t count = 0;
  object_t *iter = NULL;
  printf("[obj num: "FMT_SIXE_T", mem: "FMT_SIXE_T"] \n", manager->object_count, manager->object_pool_size);
  if(detal){
    for(iter=manager->object_pool; iter<gc_manager_object_pool_end(err, manager); iter=gc_object_next(iter)){
      printf("["FMT_SIXE_T"]: ", count++);
      object_verbose(err, iter, 1, 0, 0);
    }
  }
  printf("\n");
  return *err;
}


