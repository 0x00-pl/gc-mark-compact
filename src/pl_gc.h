#ifndef _PL_GC_H_
#define _PL_GC_H_

#include <stdio.h>

#include "pl_type.h"




typedef struct gc_manager_t_decl{
  
  object_t *object_pool;
  size_t object_pool_size;
  size_t object_pool_maxsize;
  size_t object_count;
  
  object_t ***stack_pobject_pool;
  size_t stack_pobject_pool_count;
  size_t stack_pobject_pool_maxcount;
  
  size_t cur_mark;
} gc_manager_t;
err_t *gc_manager_init(err_t **err, gc_manager_t *manager);
err_t *gc_manager_halt(err_t **err, gc_manager_t *manager);


void *mem_pack(void *ptr);

object_t *gc_object_next(object_t *obj);

object_t *gc_manager_object_pool_end(err_t **err, gc_manager_t *manager);

err_t *gc_manager_stack_object_push(err_t **err, gc_manager_t *manager, object_t** pobj);
err_t *gc_manager_stack_object_pop(err_t** err, gc_manager_t* manager, size_t stack_ptr_count);
size_t gc_manager_stack_object_get_depth(gc_manager_t *manager);
int gc_manager_stack_object_balance(gc_manager_t *manager, size_t depth);

err_t *gc_manager_object_pool_resize(err_t **err, gc_manager_t *manager, size_t new_size);

object_t *gc_manager_object_array_alloc(err_t** err, gc_manager_t* manager, enum_object_type_t obj_type, size_t count);

object_t *gc_manager_object_alloc(err_t **err, gc_manager_t *manager, enum_object_type_t obj_type);
object_t *gc_manager_object_alloc_ref(err_t **err, gc_manager_t *manager, object_t *obj);
err_t *gc_manager_object_reserve(err_t **err, gc_manager_t *manager, size_t mem_size);

object_t *gc_manager_object_array_expand(err_t **err, gc_manager_t *gcm, object_t *obj, size_t new_count);


err_t *gc_manager_mark(err_t **err, gc_manager_t *manager);

err_t *gc_manager_compact(err_t **err, gc_manager_t *manager, object_t *compact_dest, size_t dest_pool_size);

err_t *gc_gc(err_t **err, gc_manager_t *manager);

err_t *gc_verbose_object_pool(err_t **err, gc_manager_t *manager, int detal);

#endif
