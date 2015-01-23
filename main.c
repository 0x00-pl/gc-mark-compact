#include <stdio.h>
#include <stdlib.h>

#include "src/pl_gc.h"
//#include "src/pl_vector.h"
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int test_err(void*ret){
  int err = 0;
  void*tmpval;
  void*subret;
  
  while(0){
    printf("err = %d\n", err);
    goto fin;
  }
  
  
fin:
  free(tmpval);  
  if(err<0){
    free(subret);
    return err;
  }
  return 0;
}

int main(int argc, char *argv[]) {
//   return test_err(0);
  err_t *erre = NULL;
  err_t **err = &erre;
  object_int_part_t *p_int = NULL;
  object_ref_part_t *p_ref = NULL;
  
  // init gc_manager
  gc_manager_t *gc_manager = (gc_manager_t*)malloc(sizeof(gc_manager_t));
  
  gc_manager_init(err, gc_manager);
  
  // test 1+1
  printf("test 1+1:\n");
  object_t *i1 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  object_int_init(err, i1, 1);
  
  object_t *i2 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  object_int_init(err, i1, 1);
  
  object_t *i3 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  object_int_init(err, i1, object_as_int(err, i1)->value + object_as_int(err, i2)->value);
  
  
  gc_manager_root(err, gc_manager)->ptr = i3;
  
  printf("  before gc: %d mem.\n", gc_manager->object_pool_size);
  gc_verbose_object_pool(err, gc_manager);
  gc_gc(err, gc_manager);
  printf("  after gc: %d mem.\n", gc_manager->object_pool_size);
  gc_verbose_object_pool(err, gc_manager);
  
  
  // test vector
  printf("test vector:\n");
  
  i1 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  object_int_init(err, i1, 1);
  
  object_t *vec1 = gc_manager_object_alloc(err, gc_manager, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, vec1); PL_CHECK;
  
  object_vector_push(err, vec1, gc_manager, i1);
  object_vector_top(err, vec1, i1);
  object_vector_push(err, vec1, gc_manager, i1);
  object_vector_pop(err, vec1, NULL);
  object_vector_push(err, vec1, gc_manager, i1);
  object_vector_pop(err, vec1, NULL);
  object_vector_pop(err, vec1, NULL);
  object_vector_pop(err, vec1, NULL);
  
  gc_manager_root(err, gc_manager)->ptr = vec1;
  
  printf("  before gc: %d mem.\n", gc_manager->object_pool_size);
  gc_verbose_object_pool(err, gc_manager);
  gc_gc(err, gc_manager);
  printf("  after gc: %d mem.\n", gc_manager->object_pool_size);
  gc_verbose_object_pool(err, gc_manager);
  
  PL_FUNC_END_EX(,err_print(*err));
  return 0;
}
