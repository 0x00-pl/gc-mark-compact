
#include <stdlib.h>
#include "src/pl_gc.h"
#include "src/pl_parser.h"
#include "src/pl_compile.h"
#include "src/pl_vm.h"
#include "src/pl_eval_file.h"



err_t *test_gc_1(err_t **err, gc_manager_t *gcm){
  size_t gc_stack_size;
  object_t *useless = NULL;
  object_t *int_data = NULL;
  object_t *ref_to_int = NULL;
  object_t *vector_ref = NULL;
  
  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &useless); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &int_data); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &ref_to_int); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &vector_ref); PL_CHECK;
  
  useless = gc_manager_object_array_alloc(err, gcm, TYPE_INT, 10); PL_CHECK;
  useless = NULL;
  
  int_data = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  ref_to_int = gc_manager_object_alloc_ref(err, gcm, int_data); PL_CHECK;
  vector_ref = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, vector_ref); PL_CHECK;
  
  useless = gc_manager_object_array_alloc(err, gcm, TYPE_INT, 10); PL_CHECK;
  useless = NULL;
  
  // expand vectpr pdata
  object_vector_push(err, gcm, vector_ref, ref_to_int); PL_CHECK;
  object_vector_push(err, gcm, vector_ref, ref_to_int); PL_CHECK;
  object_vector_push(err, gcm, vector_ref, ref_to_int); PL_CHECK;
  object_vector_push(err, gcm, vector_ref, ref_to_int); PL_CHECK;
  object_vector_push(err, gcm, vector_ref, ref_to_int); PL_CHECK;
  
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_manager_object_pool_resize(err, gcm, gcm->object_pool_maxsize+1); PL_CHECK;
  gc_gc(err, gcm);
  
  
  object_int_init(err, int_data, 123);
  
  PL_ASSERT(vector_ref->part._vector.count==5, err_testing);
  PL_ASSERT(vector_ref->part._vector.pdata->part._ref.ptr==int_data, err_testing);
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return *err;
}


err_t *test_parsere_1(err_t **err, gc_manager_t *gcm){
  size_t gc_stack_size;
  size_t pos;
  object_t *parsed_exp = NULL;
  object_t *parsed_exp_code = NULL; 
  
  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &parsed_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &parsed_exp_code); PL_CHECK;
  
  pos = 0;
  parsed_exp = parser_parse_exp(err, gcm, "(1 2 3)", &pos); PL_CHECK;
  
  parser_verbose(err, parsed_exp); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return *err;
}

err_t *test_parsere_2(err_t **err, gc_manager_t *gcm, const char *text){
  size_t gc_stack_size;
  size_t pos;
  object_t *parsed_exp = NULL;
  object_t *parsed_exp_code = NULL; 
  
  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &parsed_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &parsed_exp_code); PL_CHECK;
  
  pos = 0;
  parsed_exp = parser_parse_exp(err, gcm, text, &pos); PL_CHECK;
  
  parser_verbose(err, parsed_exp); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return *err;
}


err_t *test_compile_1(err_t **err, gc_manager_t *gcm, const char *text){
  size_t gc_stack_size;
  size_t pos;
  object_t *parsed_exp = NULL;
  object_t *parsed_exp_code = NULL; 
  
  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &parsed_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &parsed_exp_code); PL_CHECK;
  
  pos = 0;
  parsed_exp = parser_parse_exp(err, gcm, text, &pos); PL_CHECK;
  parsed_exp_code = compile_global(err, gcm, parsed_exp); PL_CHECK;
  
  parser_verbose(err, parsed_exp); PL_CHECK;
  printf("\n");
  compile_verbose_code(err, gcm, parsed_exp_code, 0); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return *err;
}

err_t *test_vm_1(err_t **err, gc_manager_t *gcm, const char *text){
  size_t gc_stack_size;
  size_t pos;
  int done = 0;
  object_t *parsed_exp = NULL;
  object_t *parsed_exp_code = NULL; 
  object_t *vm = NULL; 
  
  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &parsed_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &parsed_exp_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  
  pos = 0;
  parsed_exp = parser_parse_exp(err, gcm, text, &pos); PL_CHECK;
  parsed_exp_code = compile_global(err, gcm, parsed_exp); PL_CHECK;
  
  parser_verbose(err, parsed_exp); PL_CHECK;
  printf("\n");
  compile_verbose_code(err, gcm, parsed_exp_code, 0); PL_CHECK;
  printf("\n === vm start === \n");
  
  
  vm = vm_alloc(err, gcm, parsed_exp_code); PL_CHECK;
  
  vm_add_stdlib(err, gcm, vm); PL_CHECK;
  
//   vm_verbose_env(err, gcm, vm); PL_CHECK;
    
  do{
//        vm_verbose_cur_code(err, gcm, vm); PL_CHECK; // debug
    done = vm_step(err, vm, gcm); PL_CHECK;
//        vm_verbose_stack(err, gcm, vm); PL_CHECK; // debug
  }while(!done);
  
  printf("\n === vm end === \n");
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return *err;
}

const char *test_code_1(){
  return "(\n\
;;; The FACT procedure computes the factorial \n\
;;; of a non-negative integer.\n\
(define fact\n\
  (lambda (n)\n\
    (if (= n 0)\n\
        1        ;Base case: return 1\n\
        (* n (fact (- n 1))))))\n\
(display (fact 4))\n\
    )";
}
const char *test_code_2(){
  return "(\n\
(cond (0 0) (1 1))\n\
\n\
    )";
}
err_t *test_file_1(err_t **err, gc_manager_t *gcm, const char *filename){
  return eval_file(err, gcm, filename);
}

int main(int argc, char **argv){
  (void)argc;(void)argv;
  gc_manager_t *gcm = NULL;
  err_t *rerr = NULL;
  err_t **err = &rerr;
  
  // init global value
  op_init_global(err); PL_CHECK;
  gcm = malloc(sizeof(gc_manager_t));
  gc_manager_init(err, gcm); PL_CHECK;
  
//   test_gc_1(err, gcm); PL_CHECK;
//   test_parsere_1(err, gcm); PL_CHECK;
//   printf("\n");
//   test_parsere_2(err, gcm, "(lambda (x) (+ x 1))"); PL_CHECK;
//   printf("\n");
//   test_compile_1(err, gcm, "((define inc (lambda (x) (+ x 1))))"); PL_CHECK;
//   
//   test_vm_1(err, gcm, "((+ 1 1))");  
//   test_vm_1(err, gcm, "((display \"hello world!\"))");  
//   test_vm_1(err, gcm, "((display (- 1 1)))");
//   test_vm_1(err, gcm, "((display (- 1)))");  
//   test_vm_1(err, gcm, "((display 1))");  
//   test_vm_1(err, gcm, "((display (+ 1 1)))");  
//   test_vm_1(err, gcm, "((display '1))");
//   test_vm_1(err, gcm, "((display 'x))");
//   test_vm_1(err, gcm, "((display '(- 1 '1)))");
//   test_vm_1(err, gcm, "((define a +) (display (a 1 1)))");  
//   test_vm_1(err, gcm, "((define b (+ 1 1)) (display b))");  
//   test_vm_1(err, gcm, "((define inc (lambda (x)(+ x 1))))");  
//   test_vm_1(err, gcm, "((define inc (lambda (x)(+ x 1))) (inc 1))");  
//   test_vm_1(err, gcm, "((define inc (lambda (x)(+ x 1))) (display (inc 1)))");
//   test_vm_1(err, gcm, "((define addn (lambda (x)(lambda (y) (+ x y)))) (define add1 (addn 1)) (display (add1 1)))");
//   test_vm_1(err, gcm, "((display (if 1 2 3)))");
//   test_vm_1(err, gcm, "((begin (display 1) (display 2) (display 3)))");
//   test_vm_1(err, gcm, test_code_1());
//   test_vm_1(err, gcm, test_code_2());
//   test_vm_1(err, gcm, "((define add (lambda (a b) (+ a b))))");  
//   test_vm_1(err, gcm, "((define (add a b) (+ a b)))");  
//   test_vm_1(err, gcm, "((define (add a b) (+ a b)) (display (add 40 2)))");  
  test_file_1(err, gcm, "test/test.lisp");
  test_file_1(err, gcm, "test/test1.lisp");
  test_file_1(err, gcm, "test/test2.lisp");
  test_file_1(err, gcm, "test/test3.lisp");
  test_file_1(err, gcm, "test/test4.lisp");
  test_file_1(err, gcm, "test/test5.lisp");
  
  gc_manager_halt(err, gcm); PL_CHECK;
  
  op_free_global(err); PL_CHECK;
  PL_FUNC_END
  err_print(*err);
  return 0;
}

