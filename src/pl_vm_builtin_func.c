#include "pl_vm_builtin_func.h"
#include "pl_op_code.h"
#include "pl_vm.h"


err_t *vm_step_op_call_make_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  size_t gcm_stack_depth;
  size_t i;
  size_t count;
  object_t *env = NULL;
  object_t *envname = NULL;
  object_t *key_value_pair = NULL;
  object_t *key_symbol = NULL;
  object_t *lambda = NULL;
  object_t *lambda_copy = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  
  PL_ASSERT(args_count==3, err_out_of_range);
  
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &envname); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda_copy); PL_CHECK;
  
  
  // init env
  env = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, env); PL_CHECK;
  key_value_pair = object_tuple_cons_alloc(err, gcm, g_nil, NULL); PL_CHECK;
  object_vector_push(err, gcm, env, key_value_pair); PL_CHECK;
  
  // build env of lambda
  envname = object_vector_ref_index(err, stack, -2); PL_CHECK;
  count = object_array_count(err, envname); PL_CHECK;
  for(i=0; i<count; i++){
    key_symbol = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
    object_symbol_init(err, key_symbol, OBJ_ARR_AT(envname, _symbol, i).name); PL_CHECK;
    key_value_pair = object_tuple_frame_resolve(err, gcm, top_frame, key_symbol); PL_CHECK;
    PL_ASSERT_NOT_NULL(key_value_pair); // cannot find symbol in current frame
    object_vector_push(err, gcm, env, key_value_pair); PL_CHECK;
  }
  key_value_pair = NULL;
  key_symbol = NULL;
  envname = NULL;
  
  lambda = object_vector_ref_index(err, stack, -1); PL_CHECK;
  lambda_copy = object_tuple_lambda_copy(err, gcm, lambda); PL_CHECK;
  object_tuple_lambda_set_env(err, lambda_copy, env); PL_CHECK;
  
  // stack pop
  // g_lambda argname lambda ==> lambda
  object_vector_pop(err, stack, NULL);
  object_vector_pop(err, stack, NULL);
  object_vector_pop(err, stack, NULL);
  object_vector_push(err, gcm, stack, gc_manager_object_alloc_ref(err, gcm, lambda_copy)); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *vm_step_op_call_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  size_t gcm_stack_depth;
  size_t i;
  object_t *args_name = NULL;
  object_t *new_frame = NULL;
  object_t *env = NULL;
  object_t *env_item = NULL;
  object_t *env_item_symbol = NULL;
  object_t *env_item_value = NULL;
    
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &args_name); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env_item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env_item_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env_item_value); PL_CHECK;
  
  args_name = object_tuple_lambda_get_argname(err, func); PL_CHECK;
  
  // env = vector(cons(g_nil,lambda.env),zip(lambda.arg_name, stack[-args_count:]))
  env = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  env_item_value = object_tuple_lambda_get_env(err, func); PL_CHECK;
  PL_ASSERT_NOT_NULL(env_item_value); PL_CHECK;
  env_item = object_tuple_cons_alloc(err, gcm, g_nil, env_item_value); PL_CHECK;
  object_vector_push(err, gcm, env, env_item); PL_CHECK;
  
  for(i=0; i<args_count; i++){
    env_item_symbol = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
    object_symbol_init(err, env_item_symbol, OBJ_ARR_AT(args_name, _symbol, i).name); PL_CHECK;
    object_vector_index(err, stack, (int)i-(int)args_count, env_item_value); PL_CHECK;
    env_item = object_tuple_cons_alloc(err, gcm, env_item_symbol, env_item_value); PL_CHECK;
    object_vector_push(err, gcm, env, env_item); PL_CHECK;
  }
  for(i=0; i<args_count; i++){
    object_vector_pop(err, stack, NULL); PL_CHECK;
  }
  
  env = object_vector_to_array(err, env, gcm); PL_CHECK;
  
  // alloc new frame
  new_frame = object_tuple_frame_alloc(err, gcm, func, env, top_frame); PL_CHECK;
  
  object_tuple_vm_set_top_frame(err, vm, new_frame); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *vm_step_op_call_define(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  (void)vm;
  size_t gcm_stack_depth;
  size_t i;
  object_t *key_value_pair = NULL;
  object_t *symbol = NULL;
  object_t *value = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
    
  symbol = object_vector_ref_index(err, stack, -2); PL_CHECK;
  value = object_vector_ref_index(err, stack, -1); PL_CHECK;
  for(i=0; i<args_count; i++){
    object_vector_pop(err, stack, NULL); PL_CHECK;
  }
  
  key_value_pair = object_tuple_frame_resolve(err, gcm, top_frame, symbol); PL_CHECK;
  if(key_value_pair == NULL){
    object_tuple_frame_add_env(err, gcm, top_frame, symbol, value); PL_CHECK;
  }else{
    object_tuple_cons_set_cdr(err, key_value_pair, value); PL_CHECK;
  }
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
// === file end ===