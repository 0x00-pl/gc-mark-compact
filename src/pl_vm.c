#include "pl_vm.h"

#include "pl_gc.h"
#include "pl_type.h"
#include "pl_err.h"
#include "pl_op_code.h"
#include "pl_vm_builtin_func.h"

// vm:{top_frame:frame(), }
object_t *vm_alloc(err_t **err, gc_manager_t *gcm, object_t *code){
  size_t gcm_stack_depth;
  object_t *vm = NULL;
  object_t *top_frame = NULL;
  object_t *lambda = NULL;
  object_t *empty_env = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &empty_env); PL_CHECK;
  
  
  vm = object_tuple_alloc(err, gcm, 1); PL_CHECK;
  empty_env = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, empty_env); PL_CHECK;
  lambda = object_tuple_empty_lambda_alloc(err, gcm, code); PL_CHECK;
  top_frame = object_tuple_frame_alloc(err, gcm, lambda, empty_env, NULL); PL_CHECK;  //inital defalut frame
  object_tuple_frame_add_env(err, gcm, top_frame, g_nil, NULL); PL_CHECK; //init NULL parent_env
  object_tuple_vm_set_top_frame(err, vm, top_frame); PL_CHECK;
  
  PL_FUNC_END_EX(,vm=NULL);
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return vm;
}

object_t *object_tuple_vm_get_top_frame(err_t **err, object_t *vm){
  (void)err;
  return OBJ_ARR_AT(vm, _ref, 0).ptr;
}

err_t *object_tuple_vm_set_top_frame(err_t **err, object_t *vm, object_t *top_frame){
  OBJ_ARR_AT(vm, _ref, 0).ptr = top_frame;
  return *err;
}


int vm_step(err_t **err, object_t *vm, gc_manager_t *gcm){
  size_t gcm_stack_depth;
  object_t *arg1 = NULL;
  object_t *cons_ar = NULL;
  object_t *cons_dr = NULL;
  object_t *cur_code = NULL;
  object_t *define_symbol = NULL;
  object_t *define_value = NULL;
  object_t *func = NULL;
  object_t *prev_frame = NULL;
  object_t *prev_stack = NULL;
  object_t *stack = NULL;
  object_t *stack_top = NULL;
  object_t *symbol_value_pair = NULL;
  object_t *top_frame = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &arg1); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &cons_ar); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &cons_dr); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &cur_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &define_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &define_value); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &func); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &prev_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &prev_stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack_top); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  stack = object_tuple_frame_get_stack(err, top_frame); PL_CHECK;
  cur_code = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
  object_tuple_frame_inc_pc(err, top_frame); PL_CHECK;
  
  if(cur_code == op_push){
    arg1 = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
    object_tuple_frame_inc_pc(err, top_frame); PL_CHECK;
    
    object_vector_push(err, gcm, stack, gc_manager_object_alloc_ref(err, gcm, arg1)); PL_CHECK;
  }else if(cur_code == op_jmp){
    arg1 = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
    object_tuple_frame_inc_pc(err, top_frame); PL_CHECK;
    object_type_check(err, arg1, TYPE_INT); PL_CHECK;
    
    object_tuple_frame_set_pc(err, top_frame, arg1->part._int.value); PL_CHECK;
  }else if(cur_code == op_jn){
    arg1 = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
    object_tuple_frame_inc_pc(err, top_frame); PL_CHECK;
    object_type_check(err, arg1, TYPE_INT); PL_CHECK;
    
    stack_top = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_vector_top(err, stack, stack_top); PL_CHECK;
    object_vector_pop(err, stack); PL_CHECK;
    if(!object_is_nil(err, stack_top->part._ref.ptr)){
      object_tuple_frame_set_pc(err, top_frame, arg1->part._int.value); PL_CHECK;
    }
  }else if(cur_code == op_call){
    arg1 = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
    object_tuple_frame_inc_pc(err, top_frame); PL_CHECK;
    object_type_check(err, arg1, TYPE_INT); PL_CHECK;
    
    
    object_type_check(err, stack->part._vector.pdata, TYPE_REF); PL_CHECK;
    func = object_vector_ref_index(err, stack, -(int)arg1->part._int.value); PL_CHECK;
    
    // built-in function
    if(func->type == TYPE_SYMBOL){
      vm_step_op_call_resolve(err, gcm, vm, top_frame, func, stack, (size_t)arg1->part._int.value); PL_CHECK;
    }
    else if(func == g_define || func == g_set){
      vm_step_op_call_define(err, gcm, vm, top_frame, func, stack, (size_t)arg1->part._int.value); PL_CHECK;
    }
    else if(func == g_mklmd){
      vm_step_op_call_make_lambda(err, gcm, vm, top_frame, func, stack, (size_t)arg1->part._int.value); PL_CHECK; 
    }
    else if(func->type == TYPE_RAW){
      // c function
      PL_ASSERT(func->part._raw.ptr!=NULL, err_null);
      ((vm_step_op_call_c_function_t)func->part._raw.ptr)(err, gcm, vm, top_frame, func, stack, (size_t)arg1->part._int.value); PL_CHECK;
    }
    else if(object_tuple_is_lambda(err, func)){
      vm_step_op_call_lambda(err, gcm, vm, top_frame, func, stack, (size_t)arg1->part._int.value);
    }
    else{
      // function is unknow 
      PL_ASSERT(0, err_typecheck);
    }
    
  }else if(cur_code == op_ret){
    prev_frame = object_tuple_frame_get_prev_frame(err, top_frame); PL_CHECK;
    if(prev_frame == NULL) {return 1;} // halt
    
    prev_stack = object_tuple_frame_get_stack(err, prev_frame); PL_CHECK;
    
    stack_top = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_vector_top(err, stack, stack_top); PL_CHECK;
    object_vector_pop(err, stack); PL_CHECK;
    
    object_vector_ref_push(err, gcm, prev_stack, stack_top); PL_CHECK;
    
    object_tuple_vm_set_top_frame(err, vm, prev_frame); PL_CHECK;
  }else{
    PL_ASSERT_EX(0, err_typecheck(PL_ERR_DEFAULT_MSG_ARGS("unknow op code")), goto fin);
  }
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return 0;
}


err_t *vm_add_stdlib(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  object_t *top_frame = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  
  add_builtin_func(err, gcm, top_frame, "+", &vm_step_op_call_addi);
  add_builtin_func(err, gcm, top_frame, "display", &vm_step_op_call_display);
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *vm_verbose_cur_code(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  object_t *top_frame = NULL;
  object_t *cur_code = NULL;
  object_t *pc = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &cur_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &pc); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  cur_code = object_tuple_frame_get_current_code(err, top_frame); PL_CHECK;
  pc = object_tuple_frame_get_pc(err, top_frame); PL_CHECK;
  
  printf("[%ld]:", pc->part._int.value);
  object_verbose(err, cur_code, 3, 0, 0);
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
err_t *vm_verbose_stack(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  object_t *top_frame = NULL;
  object_t *stack = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &stack); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  stack = object_tuple_frame_get_stack(err, top_frame); PL_CHECK;
  
  printf(" === stack ===\n");
  object_verbose(err, stack, 3, 0, 0); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
err_t *vm_verbose_env(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  object_t *top_frame = NULL;
  object_t *env = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  
  env = object_tuple_frame_get_env(err, top_frame); PL_CHECK;
  
  printf(" === env ===\n");
  object_verbose(err, env, 5, 0, 0); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
err_t *vm_verbose_frame(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  object_t *top_frame = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &top_frame); PL_CHECK;
  
  top_frame = object_tuple_vm_get_top_frame(err, vm); PL_CHECK;
  
  printf(" === top-frame ===\n");
  object_verbose(err, top_frame, 2, 0, 0); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
err_t *vm_verbose(err_t **err, gc_manager_t *gcm, object_t *vm){
  size_t gcm_stack_depth;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &vm); PL_CHECK;
  
  
  printf(" === vm ===\n");
  object_verbose(err, vm, 2, 0, 0); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}



