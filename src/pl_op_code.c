#include "pl_op_code.h"

#include <stdlib.h>
#include "pl_gc.h"
#include "pl_type.h"

err_t *op_init_global(err_t **err, gc_manager_t *gcm){
  (void)gcm;
  // define lambda if set!
  
  g_cons   = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_cons  ->size=object_sizeof(err, TYPE_STR); g_cons  ->type=TYPE_STR; object_str_init(err, g_cons   , "g_cons   "); PL_CHECK; 
  g_frame  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_frame ->size=object_sizeof(err, TYPE_STR); g_frame ->type=TYPE_STR; object_str_init(err, g_frame  , "g_frame  "); PL_CHECK; 
  g_lambda = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_lambda->size=object_sizeof(err, TYPE_STR); g_lambda->type=TYPE_STR; object_str_init(err, g_lambda , "g_lambda "); PL_CHECK; 
  g_mklmd  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_mklmd ->size=object_sizeof(err, TYPE_STR); g_mklmd ->type=TYPE_STR; object_str_init(err, g_mklmd  , "g_mklmd  "); PL_CHECK; 
  g_nil    = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_nil   ->size=object_sizeof(err, TYPE_STR); g_nil   ->type=TYPE_STR; object_str_init(err, g_nil    , "g_nil    "); PL_CHECK; 
  g_define = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_define->size=object_sizeof(err, TYPE_STR); g_define->type=TYPE_STR; object_str_init(err, g_define , "g_define "); PL_CHECK; 
  g_if     = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_if    ->size=object_sizeof(err, TYPE_STR); g_if    ->type=TYPE_STR; object_str_init(err, g_if     , "g_if     "); PL_CHECK; 
  g_set    = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_set   ->size=object_sizeof(err, TYPE_STR); g_set   ->type=TYPE_STR; object_str_init(err, g_set    , "g_set    "); PL_CHECK; 
  g_quote  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); g_quote ->size=object_sizeof(err, TYPE_STR); g_quote ->type=TYPE_STR; object_str_init(err, g_quote  , "g_quote  "); PL_CHECK; 
  
  op_call  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_call ->size=object_sizeof(err, TYPE_STR); op_call ->type=TYPE_STR; object_str_init(err, op_call  , "op_call  "); PL_CHECK; 
  op_ret   = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_ret  ->size=object_sizeof(err, TYPE_STR); op_ret  ->type=TYPE_STR; object_str_init(err, op_ret   , "op_ret   "); PL_CHECK; 
  op_find  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_find ->size=object_sizeof(err, TYPE_STR); op_find ->type=TYPE_STR; object_str_init(err, op_find  , "op_find  "); PL_CHECK; 
  op_jmp   = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_jmp  ->size=object_sizeof(err, TYPE_STR); op_jmp  ->type=TYPE_STR; object_str_init(err, op_jmp   , "op_jmp   "); PL_CHECK; 
  op_jn    = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_jn   ->size=object_sizeof(err, TYPE_STR); op_jn   ->type=TYPE_STR; object_str_init(err, op_jn    , "op_jn    "); PL_CHECK; 
  op_push  = (object_t*)malloc(object_sizeof(err, TYPE_STR)); op_push ->size=object_sizeof(err, TYPE_STR); op_push ->type=TYPE_STR; object_str_init(err, op_push  , "op_push  "); PL_CHECK; 
  
  
  PL_FUNC_END
  return *err;
}

int object_is_nil(err_t **err, object_t *obj){
  object_int_part_t *int_part = NULL;
  object_float_part_t *float_part = NULL;
  
  PL_ASSERT_NOT_NULL(obj);
  if(obj == g_nil) {return 1;}
  switch(obj->type){
    case TYPE_INT:
      int_part = object_as_int(err, obj); PL_CHECK;
      return int_part->value == 0;

    case TYPE_FLOAT:
      float_part = object_as_float(err, obj); PL_CHECK;
      return float_part->value < 0.0000001 && float_part->value > -0.0000001 /*|| isnan(float_part->value) TODO */;

    default:
      return 1;
  }
  PL_FUNC_END
  return 1;
}

object_t *object_tuple_member_index(err_t **err, object_t *tuple, size_t index){
  object_ref_part_t *part = NULL;
  PL_ASSERT(index<object_array_count(err, tuple), err_typecheck);
  part = object_as_ref(err, tuple); PL_CHECK;
  PL_FUNC_END_EX(,return NULL);
  return part[index].ptr;
}

struct gc_manager_t_decl;

object_t *object_tuple_cons_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *ar, object_t *dr){
  object_t *item = object_tuple_alloc(err, gcm, 2); PL_CHECK;
  object_member_set_value(err, item, 0, ar); PL_CHECK;
  object_member_set_value(err, item, 1, dr); PL_CHECK;
  PL_FUNC_END_EX(,item=NULL);
  return item;
}
object_t *object_tuple_cons_get_car(err_t **err, object_t *cons){
  return object_tuple_member_index(err, cons, 0);
}
err_t *object_tuple_cons_set_car(err_t **err, object_t *cons, object_t *ar){
  object_member_set_value(err, cons, 0, ar); PL_CHECK;
  PL_FUNC_END
  return *err;
}
object_t *object_tuple_cons_get_cdr(err_t **err, object_t *cons){
  return object_tuple_member_index(err, cons, 1);
}
err_t *object_tuple_cons_set_cdr(err_t **err, object_t *cons, object_t *dr){
  object_member_set_value(err, cons, 1, dr); PL_CHECK;
  PL_FUNC_END
  return *err;
}

// lambda:{op_lambda, arg_name:[symbol], exp:ref, code:[ref], env:[symbol]}
object_t *object_tuple_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *argname, object_t *exp, object_t *code, object_t *envname){
  PL_ASSERT(argname->type == TYPE_SYMBOL, err_typecheck);
  PL_ASSERT(exp->type == TYPE_REF, err_typecheck);
  PL_ASSERT(envname != NULL && envname->type == TYPE_SYMBOL, err_typecheck);
  
  object_t *new_lambda = object_tuple_alloc(err, gcm, 6); PL_CHECK;
  object_member_set_value(err, new_lambda, 0, g_lambda); PL_CHECK;
  object_member_set_value(err, new_lambda, 1, argname); PL_CHECK;
  object_member_set_value(err, new_lambda, 2, exp); PL_CHECK;
  object_member_set_value(err, new_lambda, 3, code); PL_CHECK;
  object_member_set_value(err, new_lambda, 4, envname); PL_CHECK;
  object_member_set_value(err, new_lambda, 5, NULL); PL_CHECK;
  PL_FUNC_END_EX(,new_lambda=NULL);
  return new_lambda;
}
object_t *object_tuple_lambda_copy(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda){
  return object_tuple_lambda_alloc(err, gcm, 
                                   object_tuple_lambda_get_argname(err, lambda),
                                   object_tuple_lambda_get_exp(err, lambda),
                                   object_tuple_lambda_get_code(err, lambda),
                                   object_tuple_lambda_get_envname(err, lambda));
}
int object_tuple_is_lambda(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 0) == g_lambda;
}
object_t *object_tuple_lambda_get_argname(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 1);
}
object_t *object_tuple_lambda_get_exp(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 2);
}
object_t *object_tuple_lambda_get_code(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 3);
}
object_t *object_tuple_lambda_get_envname(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 4);
}
object_t *object_tuple_lambda_get_env(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 5);
}
err_t *object_tuple_lambda_set_env(err_t **err, object_t *lambda, object_t *env){
  object_member_set_value(err, lambda, 5, env); PL_CHECK;
  PL_FUNC_END
  return *err;
}

// env:vector(cons(g_nil,prev_env), cons(symbol,object_t))
object_t *object_tuple_array_env_vector_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *prev_env){
  object_t *env_vector = NULL;
  
  size_t gs = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &prev_env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env_vector); PL_CHECK;
  
  env_vector = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, env_vector); PL_CHECK;
  object_vector_push(err, gcm, env_vector, prev_env); PL_CHECK;
  
  PL_FUNC_END_EX(,env_vector=NULL);
  gc_manager_stack_object_balance(gcm,gs);
  return env_vector;
}

// frame:{op_frame, lambda, env:env, stack:vector(ref), pc:int, prev_frame:frame()}
object_t *object_tuple_frame_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda, object_t *env, object_t *prev_frame){
  object_t *item = NULL;
  object_t *empty_stack = NULL;
  object_t *pc = NULL;
  size_t gs = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &empty_stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &pc); PL_CHECK;
  
  item = object_tuple_alloc(err, gcm, 6); PL_CHECK;
  
  empty_stack = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, empty_stack); PL_CHECK;
  
  pc = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, pc, 0); PL_CHECK;
  
  object_member_set_value(err, item, 0, g_frame); PL_CHECK;
  object_member_set_value(err, item, 1, lambda); PL_CHECK;
  object_member_set_value(err, item, 2, env); PL_CHECK;
  object_member_set_value(err, item, 3, empty_stack); PL_CHECK;
  object_member_set_value(err, item, 4, pc); PL_CHECK;
  object_member_set_value(err, item, 5, prev_frame); PL_CHECK;
  PL_FUNC_END_EX(gc_manager_stack_object_balance(gcm,gs), item=NULL);
  return item;
}
object_t *object_tuple_frame_get_lambda(err_t **err, object_t *frame){
  return object_tuple_member_index(err, frame, 1);
}
object_t *object_tuple_frame_get_env(err_t **err, object_t *frame){
  return object_tuple_member_index(err, frame, 2);
}
object_t *object_tuple_frame_get_stack(err_t **err, object_t *frame){
  return object_tuple_member_index(err, frame, 3);
}
object_t *object_tuple_frame_get_pc(err_t **err, object_t *frame){
  return object_tuple_member_index(err, frame, 4);
}
object_t *object_tuple_frame_get_prev_frame(err_t **err, object_t *frame){
  return object_tuple_member_index(err, frame, 5);
}

object_t *object_tuple_frame_get_code(err_t **err, object_t *frame, int offset){
  
  object_t *lambda = object_tuple_frame_get_lambda(err, frame); PL_CHECK;
  
  object_t *code = object_tuple_lambda_get_code(err, lambda); PL_CHECK;
  
  object_int_value_t current_pc = object_tuple_frame_get_pc(err, frame)->part._int.value; PL_CHECK;
    
  return OBJ_ARR_AT(code, _ref, current_pc + offset).ptr;
  PL_FUNC_END
  return NULL;
}
object_t *object_tuple_frame_get_current_code(err_t **err, object_t *frame){
  return object_tuple_frame_get_code(err, frame, 0);
}

err_t *object_tuple_frame_set_pc(err_t **err, object_t *frame, long int new_pc){  
  object_int_part_t *pc = object_member_int(err, frame, 3); PL_CHECK;
  pc->value = new_pc;
  PL_FUNC_END
  return *err;
}
err_t *object_tuple_frame_inc_pc(err_t **err, object_t *frame){  
  object_int_part_t *pc = object_member_int(err, frame, 3); PL_CHECK;
  pc->value++;
  PL_FUNC_END
  return *err;
}
// return key-value pair
object_t *object_tuple_env_resolve(err_t **err, gc_manager_t *gcm, object_t *env, object_t *symbol){
  size_t gcm_stack_depth;
  size_t i;
  object_t *key_value_pair = NULL;
  object_t *key = NULL;
  
  if(env == NULL) {return NULL;}
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key); PL_CHECK;
    
  object_type_check(err, env, TYPE_VECTOR); PL_CHECK;
  for(i=0; i<env->part._vector.count; i++){
    key_value_pair = ((object_ref_part_t*)object_vector_index(err, env, (int)i, NULL))->ptr; PL_CHECK;
    key = object_tuple_cons_get_car(err, key_value_pair); PL_CHECK;
    if(object_str_eq(key->part._symbol.name, symbol->part._symbol.name)){
      // found
      break;
    }
  }
  if(i >= env->part._vector.count){
    // if not found in current env
    key_value_pair = object_tuple_frame_resolve(err, gcm, object_vector_ref_index(err, env, 0), symbol);
  }
    
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return key_value_pair;
}
// return key-value pair
object_t *object_tuple_frame_resolve(err_t **err, gc_manager_t *gcm, object_t *frame, object_t *symbol){
   object_t *env = object_tuple_frame_get_env(err, frame);
   return object_tuple_env_resolve(err, gcm, env, symbol);
}

err_t *object_tuple_frame_add_env(err_t **err, gc_manager_t *gcm, object_t *frame, object_t *symbol, object_t *value){
  size_t gcm_stack_depth;
  object_t *key_value_pair = NULL;
  object_t *env = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  
  key_value_pair = object_tuple_cons_alloc(err, gcm, symbol, value); PL_CHECK;
  env = object_tuple_frame_get_env(err, frame); PL_CHECK;
  object_vector_push(err, gcm, env, key_value_pair); PL_CHECK;
  
  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}