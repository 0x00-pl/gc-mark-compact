#include <stdlib.h>
#include <string.h>

#include "pl_gc.h"




int gc_manager_init(gc_manager_t *manager){
	manager->object_pool = malloc(1024);
	manager->object_pool_maxsize = 1024;
	manager->object_pool_size = 0;
	
	// root object
	gc_manager_object_alloc(manager, TYPE_REF);
	manager->object_pool->mark = 1;
	return 0;
}

int gc_manager_halt(gc_manager_t *manager){
	object_t *iter= NULL;
	
	for(iter=manager->object_pool; iter<gc_manager_object_pool_end(manager); iter=gc_object_next(iter)){
		object_halt(iter);
	}
	free(manager->object_pool);
	return 0;
}

object_ref_part_t *gc_manager_root(gc_manager_t *manager){
	return object_as_ref(&manager->object_pool[0]);
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
object_t *gc_manager_object_pool_end(gc_manager_t *manager){
	void *ret = mem_pack(((char*)manager->object_pool) + manager->object_pool_size);
	return (object_t*)ret;
}

int gc_manager_resize_object_pool(gc_manager_t *manager, size_t new_size){
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
	if(new_object_pool == NULL) {return -1;}
	
	memmove(new_object_pool, manager->object_pool, manager->object_pool_size);
	
	manager->object_pool = new_object_pool;
	manager->object_pool_maxsize = new_maxsize;
	
	for(iter=manager->object_pool; iter<gc_manager_object_pool_end(manager); iter=gc_object_next(iter)){
		obj_rebase(iter, old_object_pool, old_object_pool_size, new_object_pool);
	}
	
	free(old_object_pool);
	return 0;
}

object_t *gc_manager_object_array_alloc(gc_manager_t *manager, enum_object_type_t obj_type, size_t size){
	object_t *new_object= NULL;
	size_t required_size;
	
	required_size = manager->object_pool_size + obj_array_sizeof(obj_type, size);
	
	if(required_size > manager->object_pool_maxsize){
		gc_manager_resize_object_pool(manager, required_size);
	}
	
	new_object = gc_manager_object_pool_end(manager);
	new_object->size = obj_array_sizeof(obj_type, size);
	new_object->type = obj_type;
	
	manager->object_pool_size = gc_object_offset(manager->object_pool, gc_object_next(new_object));
	
	return new_object;
}
object_t *gc_manager_object_alloc(gc_manager_t *manager, enum_object_type_t obj_type){
	return gc_manager_object_array_alloc(manager, obj_type, 1);
}

int gc_manager_mark(gc_manager_t *manager){
	size_t new_mark = manager->object_pool[0].mark + 1;
	obj_mark(&manager->object_pool[0], new_mark);
	return 0;
}

static object_t *gc_manager_shadow_object_pool_alloc(gc_manager_t *manager){
	return (object_t*)malloc(manager->object_pool_size);
}

int gc_manager_zip(gc_manager_t *manager){
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
	shadow_pool = gc_manager_shadow_object_pool_alloc(manager);
	
	for(iter=manager->object_pool; iter<gc_manager_object_pool_end(manager); iter=gc_object_next(iter)){
		if(iter->mark != mark) {
			object_halt(iter);
			continue;
		}
		
		
		obj_rebase(iter, manager->object_pool, object_pool_size, shadow_pool);
		
		// fill shadow_pool
		shadow_object = iter;
		obj_ptr_rebase(&shadow_object, manager->object_pool, object_pool_size, shadow_pool);
		
		shadow_object->size = obj_sizeof(TYPE_GC_BROKEN);
		shadow_object->type = TYPE_GC_BROKEN;
		broken_value = (object_gc_broken_part_t*)object_part(shadow_object);
		
		
		obj_move(iter, new_end);
		broken_value->ptr= new_end;
		
		new_end = gc_object_next(new_end);
	}
	manager->object_pool_size = gc_object_offset(manager->object_pool, new_end);
	
	for(iter=manager->object_pool; iter<gc_manager_object_pool_end(manager); iter=gc_object_next(iter)){
		obj_fix_gc_broken(iter);
	}
	
	free(shadow_pool);
	return 0;
}


int gc_gc(gc_manager_t *manager){
	gc_manager_mark(manager);
	gc_manager_zip(manager);
	return 0;
}


int gc_verbose_object_pool(gc_manager_t *manager){
	object_t *iter = NULL;
	size_t count = 0;
	for(iter=manager->object_pool; iter<gc_manager_object_pool_end(manager); iter=gc_object_next(iter)){
		printf("object%d type: %d, size: %d \n", count, iter->type, iter->size);
		count++;
	}
}


