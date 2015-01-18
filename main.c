#include <stdio.h>
#include <stdlib.h>

#include "src/pl_gc.h"
#include "src/pl_vector.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */


int main(int argc, char *argv[]) {
	object_int_part_t *p_int = NULL;
	object_ref_part_t *p_ref = NULL;
	
	// init gc_manager
	gc_manager_t *gc_manager = (gc_manager_t*)malloc(sizeof(gc_manager_t));
	
	gc_manager_init(gc_manager);
	
	// test 1+1
	printf("test 1+1:\n");
	object_t *i1 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_int_init(i1, 1);
	
	object_t *i2 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_int_init(i1, 1);
	
	object_t *i3 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_int_init(i1, object_as_int(i1)->value + object_as_int(i2)->value);
	
	
	gc_manager_root(gc_manager)->ptr = i3;
	
	printf("  before gc: %d mem.\n", gc_manager->object_pool_size);
	gc_verbose_object_pool(gc_manager);
	gc_gc(gc_manager);
	printf("  after gc: %d mem.\n", gc_manager->object_pool_size);
	gc_verbose_object_pool(gc_manager);
	
	
	// test vector
	printf("test vector:\n");
	
	i1 = gc_manager_object_alloc(gc_manager, TYPE_INT);
	object_int_init(i1, 1);
	
	object_vector_t *vec1 = object_vector_alloc(gc_manager);
	object_vector_push(vec1, gc_manager, i1);
	object_vector_top(vec1, i1);
	object_vector_push(vec1, gc_manager, i1);
	object_vector_pop(vec1, NULL);
	object_vector_push(vec1, gc_manager, i1);
	object_vector_pop(vec1, NULL);
	object_vector_pop(vec1, NULL);
	object_vector_pop(vec1, NULL);
	
	gc_manager_root(gc_manager)->ptr = vec1;
	
	printf("  before gc: %d mem.\n", gc_manager->object_pool_size);
	gc_verbose_object_pool(gc_manager);
	gc_gc(gc_manager);
	printf("  after gc: %d mem.\n", gc_manager->object_pool_size);
	gc_verbose_object_pool(gc_manager);
	
	return 0;
}
