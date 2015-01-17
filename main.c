#include <stdio.h>
#include <stdlib.h>

#include "pl_gc.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

static object_int_t *object_as_int(object_t *obj){
	object_int_t *p_int = NULL;
	p_int = (object_int_t*)gc_obj_value(obj);
	return p_int;
}
static object_ref_t *object_as_ref(object_t *obj){
	object_ref_t *p_ref = NULL;
	p_ref = (object_ref_t*)gc_obj_value(obj);
	return p_ref;
}


int main(int argc, char *argv[]) {
	object_int_t *p_int = NULL;
	object_ref_t *p_ref = NULL;
	gc_manager_t *gc_manager = (gc_manager_t*)malloc(sizeof(gc_manager_t));
	
	gc_manager_init(gc_manager);
	
	object_t *i1 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_as_int(i1)->value = 1;
	
	object_t *i2 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_as_int(i2)->value = 1;
	
	object_t *i3 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_as_int(i3)->value = object_as_int(i1)->value + object_as_int(i2)->value;
	
	object_as_ref(gc_manager_root(gc_manager))->ptr = i3;
	
	printf("before gc: %d mem.\n", gc_manager->object_pool_size);
	gc_gc(gc_manager);
	printf("after gc: %d mem.\n", gc_manager->object_pool_size);
	
	return 0;
}
