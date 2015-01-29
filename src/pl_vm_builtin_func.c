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
  object_vector_push(err, gcm, env, gc_manager_object_alloc_ref(err, gcm, key_value_pair)); PL_CHECK;
  
  // build env of lambda
  lambda = object_vector_ref_index(err, stack, -1); PL_CHECK;
  envname = object_tuple_lambda_get_envname(err, lambda); PL_CHECK;
  count = object_array_count(err, envname); PL_CHECK;
  for(i=0; i<count; i++){
    key_symbol = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
    object_symbol_init(err, key_symbol, OBJ_ARR_AT(envname, _symbol, i).name); PL_CHECK;
    key_value_pair = object_tuple_frame_resolve(err, gcm, top_frame, key_symbol); PL_CHECK;
    if(key_value_pair == NULL){
      // cannot find symbol in frame
      key_value_pair = object_tuple_cons_alloc(err, gcm, g_nil, NULL); PL_CHECK;
    }
    object_vector_push(err, gcm, env, gc_manager_object_alloc_ref(err, gcm, key_value_pair)); PL_CHECK;
  }
  key_value_pair = NULL;
  key_symbol = NULL;
  envname = NULL;
  
  lambda_copy = object_tuple_lambda_copy(err, gcm, lambda); PL_CHECK;
  object_tuple_lambda_set_env(err, lambda_copy, env); PL_CHECK;
  
  // stack pop
  // g_lambda argname lambda ==> lambda
  object_vector_pop(err, stack);
  object_vector_pop(err, stack);
  object_vector_pop(err, stack);
  object_vector_ref_push(err, gcm, stack, lambda_copy); PL_CHECK;
  
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
  object_vector_ref_push(err, gcm, env, env_item); PL_CHECK;
  
  for(i=1; i<args_count; i++){
    env_item_symbol = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
    object_symbol_init(err, env_item_symbol, OBJ_ARR_AT(args_name, _symbol, i-1).name); PL_CHECK;
    env_item_value = object_vector_ref_index(err, stack, (int)i-(int)args_count); PL_CHECK;
    env_item = object_tuple_cons_alloc(err, gcm, env_item_symbol, env_item_value); PL_CHECK;
    object_vector_ref_push(err, gcm, env, env_item); PL_CHECK;
  }
  for(i=0; i<args_count; i++){
    object_vector_pop(err, stack); PL_CHECK;
  }
    
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
    object_vector_pop(err, stack); PL_CHECK;
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

err_t *vm_step_op_call_resolve(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  (void)vm;
  size_t gcm_stack_depth;
  size_t i;
  object_t *key_value_pair = NULL;
  object_t *value = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
    
  
  key_value_pair = object_tuple_frame_resolve(err, gcm, top_frame, func); PL_CHECK;
  
  PL_ASSERT(key_value_pair!=NULL, err_out_of_range);
  
  for(i=0; i<args_count; i++){
    object_vector_pop(err, stack); PL_CHECK;
  }
  
  value = object_tuple_cons_get_cdr(err, key_value_pair); PL_CHECK;
  object_vector_push(err, gcm, stack, gc_manager_object_alloc_ref(err, gcm, value)); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
err_t *vm_step_op_call_display(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  (void)vm;
  size_t gcm_stack_depth;
  size_t i;
  object_t *value = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
    
  
  value = object_vector_ref_index(err, stack, -1); PL_CHECK;
  for(i=0; i<args_count; i++){
    object_vector_pop(err, stack); PL_CHECK;
  }
  object_verbose(err, value, 3, 0, 0); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}


err_t *vm_step_op_call_addi(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count){
  (void)vm;
  size_t gcm_stack_depth;
  size_t i;
  object_t *value = NULL;
  object_int_value_t sum = 0;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
    
  
  for(i=1; i<args_count; i++){
    value = object_vector_ref_index(err, stack, i-args_count); PL_CHECK;
    object_vector_pop(err, stack); PL_CHECK;
    object_type_check(err, value, TYPE_INT); PL_CHECK;
    sum += value->part._int.value;
  }
  object_vector_pop(err, stack); PL_CHECK;
  
  value = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, value, sum); PL_CHECK;
  object_vector_push(err, gcm, stack, gc_manager_object_alloc_ref(err, gcm, value)); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *add_builtin_func(err_t **err, gc_manager_t *gcm, object_t* frame, const char *str_name, vm_step_op_call_c_function_t builtin_func){
  size_t gcm_stack_depth;
  object_t *name = NULL;
  object_t *symbol = NULL;
  object_t *value = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &name); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
  
  name = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  object_str_init(err, name, str_name); PL_CHECK;
  
  symbol = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
  object_symbol_init(err, symbol, name); PL_CHECK;
  
  value = gc_manager_object_alloc(err, gcm, TYPE_RAW); PL_CHECK;
  object_raw_init(err, value, builtin_func, 0);
  
  object_tuple_frame_add_env(err, gcm, frame, symbol, value); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}



