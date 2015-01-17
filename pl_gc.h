#ifndef _PL_GC_H_
#define _PL_GC_H_

#include <stdio.h>

#include "pl_type.h"




typedef struct{
	object_t *object_pool;
	size_t object_pool_size;
	size_t object_pool_maxsize;
} gc_manager_t;
int gc_manager_init(gc_manager_t *manager);
int gc_manager_halt(gc_manager_t *manager);
object_t *gc_manager_root(gc_manager_t *manager);


void *mem_pack(void *ptr);

object_t *gc_object_next(object_t *obj);
object_t *gc_manager_object_pool_end(gc_manager_t *manager);

int gc_manager_resize_object_pool(gc_manager_t *manager, size_t new_size);

object_t *gc_manager_object_array_alloc(gc_manager_t *manager, enum_object_type_t obj_type, size_t size);

object_t *gc_manager_object_alloc(gc_manager_t *manager, enum_object_type_t obj_type);

int gc_manager_mark(gc_manager_t *manager);

int gc_manager_zip(gc_manager_t *manager);

int gc_gc(gc_manager_t *manager);

#endif
