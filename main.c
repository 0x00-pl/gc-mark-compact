/* ***this file is useless*** */
/* ***this file is useless*** */
/* ***this file is useless*** */

#include <stdio.h>
#include <stdlib.h>

#include "src/pl_gc.h"
#include "src/pl_parser.h"
#include "src/pl_op_code.h"
#include "src/pl_compile.h"
#include "src/pl_vm.h"


/* run this program using the console pauser or add your own getch, system("pause") or input loop */


err_t *run_code(err_t **err, gc_manager_t *gcm, const char *text){
  size_t gc_stack_depth;
  object_t *ast = NULL;
  object_t *parsed_exp_code = NULL;
  object_t *new_vm = NULL;
  size_t pos;
  int done;

  gc_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ast); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &parsed_exp_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_vm); PL_CHECK;

  pos = 0;
  ast = parser_parse_exp(err, gcm, text, &pos); PL_CHECK;

  parser_verbose(err, ast); PL_CHECK;
  printf("\n === end === \n\n");

	parsed_exp_code = compile_global(err, gcm, ast); PL_CHECK;

  printf("\n === code === \n");
  compile_verbose_code(err, gcm, parsed_exp_code, 0); PL_CHECK;
  printf("\n === code end === \n\n");

  new_vm = vm_alloc(err, gcm, parsed_exp_code); PL_CHECK;

  printf("\n === vm start === \n");
  done = 0;
	do{
  	vm_step(err, new_vm, gcm); PL_CHECK;
  }while(!done);
  printf("\n === vm end === \n");

  PL_FUNC_END
  gc_manager_stack_object_balance(gcm,gc_stack_depth);
  return *err;
}


int nnmain(int argc, char *argv[]) {
  (void)argc;(void)argv;
	err_t *derr = NULL;
	err_t **err = &derr;
	gc_manager_t *gcm = NULL;
	size_t gc_stack_depth;

	gcm = malloc(sizeof(gc_manager_t));
	gc_manager_init(err, gcm); PL_CHECK;
	gc_stack_depth = gc_manager_stack_object_get_depth(gcm);

	op_init_global(err); PL_CHECK;

  run_code(err, gcm, "((display 1))"); PL_CHECK;


  op_free_global(err); PL_CHECK;

  PL_FUNC_END
  gc_manager_stack_object_balance(gcm,gc_stack_depth);
  system("pause");
  return derr==NULL ? 0 : -1 ;
}


int nmain(int argc, char *argv[]) {
  (void)argc;(void)argv;
//   return test_err(0);
  err_t *erre = NULL;
  err_t **err = &erre;
//   object_int_part_t *p_int = NULL;
//   object_ref_part_t *p_ref = NULL;

  // init gc_manager
  gc_manager_t *gc_manager = (gc_manager_t*)malloc(sizeof(gc_manager_t));

  gc_manager_init(err, gc_manager);
  size_t gs = gc_manager_stack_object_get_depth(gc_manager);


  // test 1+1
  printf("test 1+1:\n");
  object_t *i1 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  gc_manager_stack_object_push(err, gc_manager, &i1);
  object_int_init(err, i1, 1);

  object_t *i2 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  gc_manager_stack_object_push(err, gc_manager, &i2);
  object_int_init(err, i2, 1);

  object_t *i3 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  gc_manager_stack_object_push(err, gc_manager, &i3);
  object_int_init(err, i3, i1->part._int.value + i2->part._int.value);



  printf("  before gc:\n");
  gc_verbose_object_pool(err, gc_manager, 0);
  gc_gc(err, gc_manager);
  printf("  after gc\n");
  gc_verbose_object_pool(err, gc_manager, 0);


  // test vector
  printf("test vector:\n");

  i1 = gc_manager_object_alloc(err, gc_manager, TYPE_INT);
  object_int_init(err, i1, 1);

  object_t *vec1 = gc_manager_object_alloc(err, gc_manager, TYPE_VECTOR); PL_CHECK;
  gc_manager_stack_object_push(err, gc_manager, &vec1);
  object_vector_init(err, vec1); PL_CHECK;

  object_vector_push(err, gc_manager, vec1, i1);
  object_vector_top(err, vec1, i1);
  object_vector_push(err, gc_manager, vec1, i1);
  object_vector_pop(err, vec1);
  object_vector_push(err, gc_manager, vec1, i1);
  object_vector_pop(err, vec1);
  object_vector_pop(err, vec1);
  object_vector_pop(err, vec1);


  printf("  before gc:\n");
  gc_verbose_object_pool(err, gc_manager, 0);
  gc_gc(err, gc_manager);
  printf("  after gc\n");
  gc_verbose_object_pool(err, gc_manager, 0);


  // parser
  printf("test parser:\n");
  size_t pos = 0;
  object_t *parsed_exp = NULL;
  object_t *parsed_exp_code = NULL;

  parsed_exp = parser_parse_exp(err, gc_manager, "((lambda () (;;; The FACT procedure computes the factorial \n\
;;; of a non-negative integer.\n\
(define fact\n\
  (lambda (n)\n\
    (if (= n 0)\n\
        1        ;Base case: return 1\n\
        (* n (fact (- n 1)))))))))", &pos);
  parser_verbose(err, parsed_exp); PL_CHECK;
  printf("\n === end === \n\n");



  gc_manager_stack_object_push(err, gc_manager, &parsed_exp_code);
  parsed_exp_code = compile_global(err, gc_manager, parsed_exp);
  printf("\n === code === \n");
  compile_verbose_code(err, gc_manager, parsed_exp_code, 0); PL_CHECK;
  printf("\n === code end === \n\n");

  printf("  before gc:\n");
  gc_verbose_object_pool(err, gc_manager, 0);
  gc_gc(err, gc_manager);
  printf("  after gc\n");
  gc_verbose_object_pool(err, gc_manager, 0);

  PL_FUNC_END_EX(, err_print(*err));
  gc_manager_stack_object_balance(gc_manager,gs);
  gc_manager_halt(err, gc_manager);
  return 0;
}
