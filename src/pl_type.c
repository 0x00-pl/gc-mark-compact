#include "pl_type.h"

#include <stdlib.h>
#include <string.h>


int object_raw_init(object_t *thiz, void* ptr){
	object_as_raw(thiz)->ptr = ptr;
	return 0;
}
int object_raw_halt(object_t *thiz){
	free(object_as_raw(thiz)->ptr);
	return 0;
}
int object_int_init(object_t *thiz, long int value){
	object_as_int(thiz)->value = value;
	return 0;
}
int object_int_halt(object_t *thiz){
	return 0;
}
int object_float_init(object_t *thiz, double value){
	object_as_float(thiz)->value = value;
	return 0;
}
int object_float_halt(object_t *thiz){
	return 0;
}
int object_str_init(object_t *thiz, const char* text){
	object_str_part_t *str_part = object_as_str(thiz);
	
	str_part->size = strlen(text);
	str_part->str = malloc(str_part->size + 1);
	memcpy(str_part->str, text, str_part->size);
	str_part->str[str_part->size] = '\0';
	return 0;
}
int object_str_halt(object_t *thiz){
	free(object_as_str(thiz)->str);
	return 0;
}
int object_symbol_init(object_t *thiz, const char* name){
	object_symbol_part_t *symbol_part = object_as_symbol(thiz);
	
	symbol_part->size = strlen(name);
	symbol_part->str = malloc(symbol_part->size + 1);
	memcpy(symbol_part->str, name, symbol_part->size);
	symbol_part->str[symbol_part->size] = '\0';
	return 0;
}
int object_symbol_halt(object_t *thiz){
	free(object_as_symbol(thiz)->str);
	return 0;
}
int object_gc_broken_init(object_t *thiz, void* ptr){
	object_as_gc_broken(thiz)->ptr = (object_t*)ptr;
	return 0;
}
int object_gc_broken_halt(object_t *thiz){
	return 0;
}
int object_ref_init(object_t *thiz, void* ptr){
	object_as_ref(thiz)->ptr = (object_t*)ptr;
	return 0;
}
int object_ref_halt(object_t *thiz){
	return 0;
}

int object_halt(object_t *obj){
	switch(obj->type){
	case TYPE_RAW:
		object_raw_halt(obj);
		break;
	case TYPE_INT:
		object_int_halt(obj);
		break;
	case TYPE_FLOAT:
		object_float_halt(obj);
		break;
	case TYPE_STR:
		object_str_halt(obj);
		break;
	case TYPE_SYMBOL:
		object_symbol_halt(obj);
		break;
	case TYPE_GC_BROKEN:
		object_gc_broken_halt(obj);
		break;
	case TYPE_EXTRA:
		// extra halt
		break;
	case TYPE_REF:
		object_ref_halt(obj);
		break;
	default:
		return -1;
	}
	return 0;
}


// object cast

int object_type_check(object_t *obj, enum_object_type_t type){
	return obj->type == type;
}
void* object_part(object_t *obj){
	size_t obj_addr = (size_t)obj;
	return (void*)(obj_addr + sizeof(object_t));
}

object_raw_part_t *object_as_raw(object_t *obj){
	if(!object_type_check(obj,TYPE_RAW)){
		return NULL;
	}
	return (object_raw_part_t*)object_part(obj);
}

object_int_part_t *object_as_int(object_t *obj){
	if(!object_type_check(obj,TYPE_INT)){
		return NULL;
	}
	return (object_int_part_t*)object_part(obj);
}

object_float_part_t *object_as_float(object_t *obj){
	if(!object_type_check(obj,TYPE_FLOAT)){
		return NULL;
	}
	return (object_float_part_t*)object_part(obj);
}

object_str_part_t *object_as_str(object_t *obj){
	if(!object_type_check(obj,TYPE_STR)){
		return NULL;
	}
	return (object_str_part_t*)object_part(obj);
}

object_symbol_part_t *object_as_symbol(object_t *obj){
	if(!object_type_check(obj,TYPE_SYMBOL)){
		return NULL;
	}
	return (object_symbol_part_t*)object_part(obj);
}

object_gc_broken_part_t *object_as_gc_broken(object_t *obj){
	if(!object_type_check(obj,TYPE_GC_BROKEN)){
		return NULL;
	}
	return (object_gc_broken_part_t*)object_part(obj);
}

object_extra_part_t *object_as_extra(object_t *obj){
	if(!object_type_check(obj,TYPE_EXTRA)){
		return NULL;
	}
	return (object_extra_part_t*)object_part(obj);
}

object_ref_part_t *object_as_ref(object_t *obj){
	if(!object_type_check(obj,TYPE_REF)){
		return NULL;
	}
	return (object_ref_part_t*)object_part(obj);
}

// object size

size_t obj_sizeof_value(enum_object_type_t obj_type){
	switch(obj_type){
	case TYPE_RAW:       return sizeof(object_raw_part_t);
	case TYPE_INT:       return sizeof(object_int_part_t);
	case TYPE_FLOAT:     return sizeof(object_float_part_t);
	case TYPE_STR:       return sizeof(object_str_part_t);
	case TYPE_SYMBOL:    return sizeof(object_symbol_part_t);
	case TYPE_GC_BROKEN: return sizeof(object_gc_broken_part_t);
	case TYPE_EXTRA:     return sizeof(object_extra_part_t);
	case TYPE_REF:       return sizeof(object_ref_part_t);
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

size_t obj_array_size(object_t *obj){
	size_t value_size = obj->size - sizeof(object_header_t);
	return value_size / obj_sizeof_value(obj->type);
}


// object gc support

int obj_mark(object_t *obj, int mark){
	object_ref_part_t *value_ref = NULL;
	size_t count;
	
	if(obj->mark == mark) {return 0;}
	
	obj->mark = mark;
	
	count = obj_array_size(obj);
	
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
		value_ref = (object_ref_part_t*)object_part(obj);
		while(count --> 0){
			obj_mark(value_ref[count].ptr, mark);
		}
		break;
	default:
		break;
	}
	
	return 0;
}


int obj_move(object_t *obj_old, object_t *obj_new){
	if(obj_old == obj_new) {return 0;}
	memmove(obj_new, obj_old, obj_old->size);
	// TODO init and halt
	return 0;
}


int obj_ptr_fix_gc_broken(object_t **pobj){
	object_gc_broken_part_t *broken_value = NULL;
	
	if((*pobj)->type == TYPE_GC_BROKEN){
		broken_value = (object_gc_broken_part_t*)object_part(*pobj);
		*pobj = broken_value->ptr;
	}
	return 0;
}
int obj_fix_gc_broken(object_t *obj){
	object_ref_part_t *value_ref = NULL;
	size_t count = obj_array_size(obj);
	
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
		value_ref = (object_ref_part_t*)object_part(obj);
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
	size_t count = obj_array_size(obj);
	object_ref_part_t *value_ref = NULL;
	
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
		value_ref = (object_ref_part_t*)object_part(obj);
		while(count --> 0){
			obj_ptr_rebase(&(value_ref[count].ptr), old_pool, old_pool_size, new_pool);
		}
		break;
	default:
		break;
	}
	
}
