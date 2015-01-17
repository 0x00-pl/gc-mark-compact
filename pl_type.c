#include "pl_type.h"

#include <stdlib.h>
#include <string.h>

size_t obj_sizeof_value(enum_object_type_t obj_type){
	switch(obj_type){
	case TYPE_RAW:       return sizeof(object_raw_t);
	case TYPE_INT:       return sizeof(object_int_t);
	case TYPE_FLOAT:     return sizeof(object_float_t);
	case TYPE_STR:       return sizeof(object_str_t);
	case TYPE_SYMBOL:    return sizeof(object_symbol_t);
	case TYPE_GC_BROKEN: return sizeof(object_gc_broken_t);
	case TYPE_EXTRA:     return sizeof(object_extra_t);
	case TYPE_REF:       return sizeof(object_ref_t);
	default:             return 0;
	}
	return 0;
}

size_t obj_array_sizeof(enum_object_type_t obj_type, size_t n){
	return sizeof(object_header_t) + n * obj_sizeof_value(obj_type);
}

size_t obj_sizeof(enum_object_type_t obj_type){
	return obj_array_sizeof(obj_type, 1);
}

size_t gc_object_array_size(object_t *obj){
	size_t value_size = obj->size - sizeof(object_header_t);
	return value_size / obj_sizeof_value(obj->type);
}

int free_raw(void* raw){
	free(raw);
	return 0;
}

int obj_free(object_t *obj){
	object_raw_t *value_raw = NULL;
	
	switch(obj->type){
	case TYPE_RAW:
		value_raw = (object_raw_t*)gc_obj_value(obj);
		free_raw(value_raw->ptr);
		break;
	case TYPE_EXTRA:
		// extra-free
		break;	
	default:
		break;
	}
	return 0;
}

int obj_mark(object_t *obj, int mark){
	object_ref_t *value_ref = NULL;
	size_t count;
	
	if(obj->mark == mark) {return 0;}
	
	obj->mark = mark;
	
	count = gc_object_array_size(obj);
	
	switch(obj->type){
	case TYPE_RAW:
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_STR:
	case TYPE_SYMBOL:
		break;
	case TYPE_EXTRA:
		// extra-mark
		break;
	case TYPE_REF:
		value_ref = (object_ref_t*)gc_obj_value(obj);
		while(count --> 0){
			obj_mark(value_ref[count].ptr, mark);
		}
		break;
	default:
		break;
	}
	
	return 0;
}


void* gc_obj_value(object_t *obj){
	size_t obj_addr = (size_t)obj;
	return (void*)(obj_addr + sizeof(object_t));
}

int obj_move(object_t *obj_old, object_t *obj_new){
	if(obj_old == obj_new) {return 0;}
	memmove(obj_new, obj_old, obj_old->size);
	// TODO
	return 0;
}


int obj_ptr_fix_gc_broken(object_t **pobj){
	object_gc_broken_t *broken_value = NULL;
	
	if((*pobj)->type == TYPE_GC_BROKEN){
		broken_value = (object_gc_broken_t*)gc_obj_value(*pobj);
		*pobj = broken_value->ptr;
	}
	return 0;
}
int obj_fix_gc_broken(object_t *obj){
	object_ref_t *value_ref = NULL;
	size_t count = gc_object_array_size(obj);
	
	switch(obj->type){
	case TYPE_RAW:
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_STR:
	case TYPE_SYMBOL:
		break;
	case TYPE_EXTRA:
		// extra-fix_gc_broken
		break;
	case TYPE_REF:
		value_ref = (object_ref_t*)gc_obj_value(obj);
		while(count --> 0){
			obj_ptr_fix_gc_broken(&(value_ref[count].ptr));
		}
		break;
	default:
		break;
	}
	return 0;
}


int obj_ptr_rebase(object_t **pobj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
	size_t old_pool_addr = (size_t)old_pool;
	size_t old_pool_end_addr = (size_t)old_pool + old_pool_size;
	size_t new_pool_addr = (size_t)new_pool;
	size_t pobj_addr = (size_t)*pobj;
	size_t new_pobj_addr;
	
	if(pobj_addr<old_pool_addr || old_pool_end_addr<=pobj_addr) {return 0;}
	
	new_pobj_addr = pobj_addr - old_pool_addr + new_pool_addr;
	
	*pobj = (object_t*)new_pobj_addr;
	
	return 0;
}

int obj_rebase(object_t *obj, object_t *old_pool, size_t old_pool_size, object_t *new_pool){
	size_t count = gc_object_array_size(obj);
	object_ref_t *value_ref = NULL;
	
	switch(obj->type){
	case TYPE_RAW:
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_STR:
	case TYPE_SYMBOL:
		break;
	case TYPE_EXTRA:
		// extra-rebase
		break;
	case TYPE_REF:
		value_ref = (object_ref_t*)gc_obj_value(obj);
		while(count --> 0){
			obj_ptr_rebase(&(value_ref[count].ptr), old_pool, old_pool_size, new_pool);
		}
		break;
	default:
		break;
	}
	
}
