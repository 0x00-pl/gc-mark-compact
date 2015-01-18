#include "pl_vector.h"

#include <stdlib.h>
#include <string.h>



// vector data resize
int object_resize(object_t **pobj, gc_manager_t *gcm, size_t new_size){
	object_t *new_array = NULL;
	size_t new_array_size;
	size_t copy_size;
	size_t old_size = obj_array_size(*pobj);
	if(old_size == new_size) {return 0;}
	
	new_array = gc_manager_object_array_alloc(gcm, (*pobj)->type, new_size);
	
	new_array_size = new_array->size;
	
	copy_size = (*pobj)->size < new_array->size ? (*pobj)->size : new_array->size;
	
	memcpy(new_array, *pobj, copy_size);
	
	new_array->size = new_array_size;
	
	*pobj = new_array;
	return 0;
}

// vector member
object_vector_t *object_vector_alloc(gc_manager_t *gcm){
	object_vector_t *ret = NULL;
	ret = gc_manager_object_array_alloc(gcm, TYPE_REF, 2);
	
	// size
	object_as_ref(ret)[0].ptr = gc_manager_object_alloc(gcm, TYPE_INT);
	object_int_init(object_as_ref(ret)[0].ptr, 0);
	
	// data
	object_as_ref(ret)[1].ptr = NULL;
	
	return ret;
}

object_int_part_t *object_vector_size(object_t *vector){
	if(object_as_ref(vector) == NULL) {return NULL;}
	return object_as_int(object_as_ref(vector)[0].ptr);
}
object_ref_part_t *object_vector_data(object_t *vector){
	if(object_as_ref(vector) == NULL) {return NULL;}
	return &object_as_ref(vector)[1];
}
size_t object_vector_maxsize(object_t *vector){
	return obj_array_size(object_vector_data(vector)->ptr);
}
enum_object_type_t object_vector_data_type(object_t *vector){
	if(object_vector_data(vector)->ptr == NULL) {return TYPE_UNKNOW;}
	return object_vector_data(vector)->ptr->type;
}

// vector push/pop
int object_vector_push(object_t *vector, gc_manager_t *gcm, object_t *value){
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
	
	if(object_vector_size(vector)->value == 0){
		// reset vector data type
		object_vector_data(vector)->ptr = gc_manager_object_array_alloc(gcm, value->type, 1);
	}
	
	m_size = object_vector_size(vector);	
	m_data = object_vector_data(vector);
	maxsize = object_vector_maxsize(vector);
	
	// vector data type check
	if(object_vector_data_type(vector) != value->type) {return -1;}
	
	// vector maxsize check
	if(m_size->value+1 >= maxsize){
		object_resize(&(object_vector_data(vector)->ptr), gcm, maxsize * 2);
	}
	
	// copy into vector
	src = obj_array_index(value, 0);
	dst = obj_array_index(m_data->ptr, m_size->value);
	memcpy(dst, src, obj_sizeof_part(value->type));
	
	m_size->value += 1;
	
	return 0;
}

int object_vector_top(object_t *vector, object_t *out_value){
	object_int_part_t *m_size;
	object_ref_part_t *m_data;
	void *src;
	void *dst;
	
	m_size = object_vector_size(vector);
	if(m_size->value <= 0) {return -1;}
	m_data = object_vector_data(vector);
	
	if(out_value != NULL){
		out_value->size = obj_sizeof(m_data->ptr->type);
		out_value->type = object_vector_data_type(vector);
		
		src = obj_array_index(m_data->ptr, m_size->value - 1);
		dst = obj_array_index(out_value, 0);
		memcpy(dst, src, obj_sizeof_part(object_vector_data_type(vector)));
	}
	return 0;
}

int object_vector_pop(object_t *vector, object_t *out_value){
	object_int_part_t *m_size;
	
	m_size = object_vector_size(vector);	
	if(m_size->value <= 0) {return -1;}
	
	if(object_vector_top(vector, out_value) != 0) {return -1;}
	
	m_size->value -= 1;

	return 0;
}

