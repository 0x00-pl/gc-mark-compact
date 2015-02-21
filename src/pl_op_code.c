#include "pl_op_code.h"

#include <stdlib.h>
#include <math.h>
#include "pl_gc.h"
#include "pl_type.h"

static object_t *make_global_symbol(err_t **err, const char *text){
  object_t *new_symbol_str = (object_t*)malloc(object_sizeof(err, TYPE_STR)); PL_CHECK;
  object_t *new_symbol = (object_t*)malloc(object_sizeof(err, TYPE_SYMBOL)); PL_CHECK;
  
  new_symbol_str->mark = 0;
  new_symbol_str->move_dest = NULL;
  new_symbol_str->size = object_sizeof(err, TYPE_STR); PL_CHECK;
  new_symbol_str->type = TYPE_STR;
  object_str_init(err, new_symbol_str, text); PL_CHECK;
  
  new_symbol->mark = 0;
  new_symbol->move_dest = NULL;
  new_symbol->size = object_sizeof(err, TYPE_SYMBOL); PL_CHECK;
  new_symbol->type = TYPE_SYMBOL;
  object_symbol_init(err, new_symbol, new_symbol_str); PL_CHECK;
  
  PL_FUNC_END_EX(
    ,
    free(new_symbol_str);
    new_symbol_str = NULL;
    free(new_symbol);
    new_symbol = NULL;)
  
  return new_symbol;
}
static int free_global_symbol(err_t **err, object_t *global_symbol){
  object_t *symbol_str = global_symbol->part._symbol.name;
  object_halt(err, symbol_str); PL_CHECK;
  free(symbol_str);
  object_halt(err, global_symbol); PL_CHECK;
  free(global_symbol);
  PL_FUNC_END
  return 0;
}

err_t *op_init_global(err_t **err){
  // define lambda if set!

  g_cons   = make_global_symbol(err, "g_cons   "); PL_CHECK;
  g_frame  = make_global_symbol(err, "g_frame  "); PL_CHECK;
  g_lambda = make_global_symbol(err, "g_lambda "); PL_CHECK;
  g_mklmd  = make_global_symbol(err, "g_mklmd  "); PL_CHECK;
  g_eval   = make_global_symbol(err, "g_eval   "); PL_CHECK;
  g_nil    = make_global_symbol(err, "g_nil    "); PL_CHECK;
  g_true   = make_global_symbol(err, "g_true   "); PL_CHECK;
  g_define = make_global_symbol(err, "g_define "); PL_CHECK;
  g_if     = make_global_symbol(err, "g_if     "); PL_CHECK;
  g_set    = make_global_symbol(err, "g_set    "); PL_CHECK;
  g_quote  = make_global_symbol(err, "g_quote  "); PL_CHECK;

  op_call  = make_global_symbol(err, "op_call  "); PL_CHECK;
  op_ret   = make_global_symbol(err, "op_ret   "); PL_CHECK;
  op_find  = make_global_symbol(err, "op_find  "); PL_CHECK;
  op_jmp   = make_global_symbol(err, "op_jmp   "); PL_CHECK;
  op_jn    = make_global_symbol(err, "op_jn    "); PL_CHECK;
  op_push  = make_global_symbol(err, "op_push  "); PL_CHECK;
  op_pop   = make_global_symbol(err, "op_pop   "); PL_CHECK;

  PL_FUNC_END
  return *err;
}
err_t *op_free_global(err_t **err){
  free_global_symbol(err, g_cons  ); PL_CHECK;
  free_global_symbol(err, g_frame ); PL_CHECK;
  free_global_symbol(err, g_lambda); PL_CHECK;
  free_global_symbol(err, g_mklmd ); PL_CHECK;
  free_global_symbol(err, g_eval  ); PL_CHECK;
  free_global_symbol(err, g_nil   ); PL_CHECK;
  free_global_symbol(err, g_true  ); PL_CHECK;
  free_global_symbol(err, g_define); PL_CHECK;
  free_global_symbol(err, g_if    ); PL_CHECK;
  free_global_symbol(err, g_set   ); PL_CHECK;
  free_global_symbol(err, g_quote ); PL_CHECK;
  
  free_global_symbol(err, op_call ); PL_CHECK;
  free_global_symbol(err, op_ret  ); PL_CHECK;
  free_global_symbol(err, op_find ); PL_CHECK;
  free_global_symbol(err, op_jmp  ); PL_CHECK;
  free_global_symbol(err, op_jn   ); PL_CHECK;
  free_global_symbol(err, op_push ); PL_CHECK;
  free_global_symbol(err, op_pop  ); PL_CHECK;
  
  PL_FUNC_END
  return *err;
}

int object_is_nil(err_t **err, object_t *obj){
  PL_ASSERT_NOT_NULL(obj);
  if(obj == g_nil) {return 1;}
  if(obj == g_true) {return 0;}
  if(object_array_count(err,obj)==0) {return 1;}
  switch(obj->type){
    case TYPE_INT:
      return obj->part._int.value == 0;
    case TYPE_FLOAT:
      return ( obj->part._float.value < 0.0000001 && obj->part._float.value > -0.0000001 ) || isnan(obj->part._float.value);
    case TYPE_STR:
      return obj->part._str.size == 0;
    case TYPE_SYMBOL:
      return obj->part._symbol.name == NULL || obj->part._symbol.name->part._str.size == 0;
    default:
      return 1;
  }
  PL_FUNC_END
  return 1;
}

object_t *object_tuple_member_index(err_t **err, object_t *tuple, size_t index){
  size_t count;
  count = object_array_count(err, tuple);
  PL_ASSERT(index<=count, err_out_of_range);
  PL_FUNC_END
  return OBJ_ARR_AT(tuple, _ref, index).ptr;
}

struct gc_manager_t_decl;

object_t *object_tuple_cons_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *ar, object_t *dr){
  size_t gcm_stack_depth;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ar); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &dr); PL_CHECK;

  object_t *item = object_tuple_alloc(err, gcm, 2); PL_CHECK;
  OBJ_ARR_AT(item, _ref, 0).ptr = ar;
  OBJ_ARR_AT(item, _ref, 1).ptr = dr;

  PL_FUNC_END_EX(,item=NULL);
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return item;
}
object_t *object_tuple_cons_get_car(err_t **err, object_t *cons){
  (void)err;
  return OBJ_ARR_AT(cons, _ref, 0).ptr;
}
err_t *object_tuple_cons_set_car(err_t **err, object_t *cons, object_t *ar){
  object_type_check(err, cons, TYPE_REF); PL_CHECK;
  OBJ_ARR_AT(cons, _ref, 0).ptr = ar;
  PL_FUNC_END
  return *err;
}
object_t *object_tuple_cons_get_cdr(err_t **err, object_t *cons){
  (void)err;
  return OBJ_ARR_AT(cons, _ref, 1).ptr;
}
err_t *object_tuple_cons_set_cdr(err_t **err, object_t *cons, object_t *dr){
  object_type_check(err, cons, TYPE_REF); PL_CHECK;
  OBJ_ARR_AT(cons, _ref, 1).ptr = dr;
  PL_FUNC_END
  return *err;
}

// lambda:{op_lambda, arg_name:[symbol], exp:ref, code:[ref], envname:[symbol], env:vector()}
object_t *object_tuple_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *argname, object_t *exp, object_t *code, object_t *envname){
  size_t gcm_stack_depth;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &argname); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &envname); PL_CHECK;

  PL_ASSERT(argname->type == TYPE_SYMBOL, err_typecheck);
  PL_ASSERT(exp->type == TYPE_REF, err_typecheck);
//   PL_ASSERT(envname != NULL && envname->type == TYPE_SYMBOL, err_typecheck); // un pass with empty symbol array

  object_t *new_lambda = object_tuple_alloc(err, gcm, 6); PL_CHECK;
  OBJ_ARR_AT(new_lambda, _ref, 0).ptr = g_lambda;
  OBJ_ARR_AT(new_lambda, _ref, 1).ptr = argname;
  OBJ_ARR_AT(new_lambda, _ref, 2).ptr = exp;
  OBJ_ARR_AT(new_lambda, _ref, 3).ptr = code;
  OBJ_ARR_AT(new_lambda, _ref, 4).ptr = envname;
  OBJ_ARR_AT(new_lambda, _ref, 5).ptr = NULL;

  PL_FUNC_END_EX(,new_lambda=NULL);
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return new_lambda;
}
object_t *object_tuple_empty_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *code){
  size_t gcm_stack_depth;
  object_t *new_lambda = NULL;
  object_t *new_empty_symbol_arr = NULL;
  object_t *new_empty_symbol_arr2 = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_lambda); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_empty_symbol_arr); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &new_empty_symbol_arr2); PL_CHECK;

  new_lambda = object_tuple_alloc(err, gcm, 6); PL_CHECK;
  new_empty_symbol_arr = gc_manager_object_array_alloc(err, gcm, TYPE_SYMBOL, 0);
  new_empty_symbol_arr2 = gc_manager_object_array_alloc(err, gcm, TYPE_SYMBOL, 0);
  OBJ_ARR_AT(new_lambda, _ref, 0).ptr = g_lambda;
  OBJ_ARR_AT(new_lambda, _ref, 1).ptr = new_empty_symbol_arr;
  OBJ_ARR_AT(new_lambda, _ref, 2).ptr = NULL;
  OBJ_ARR_AT(new_lambda, _ref, 3).ptr = code;
  OBJ_ARR_AT(new_lambda, _ref, 4).ptr = new_empty_symbol_arr2;
  OBJ_ARR_AT(new_lambda, _ref, 5).ptr = NULL;

  PL_FUNC_END_EX(,new_lambda=NULL);
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
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
  (void)err;
  object_type_check(err, env, TYPE_VECTOR);
  OBJ_ARR_AT(lambda, _ref, 5).ptr = env;
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
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &prev_frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &empty_stack); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &pc); PL_CHECK;

  item = object_tuple_alloc(err, gcm, 6); PL_CHECK;

  empty_stack = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, empty_stack); PL_CHECK;

  pc = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, pc, 0); PL_CHECK;

  OBJ_ARR_AT(item, _ref, 0).ptr = g_frame;
  OBJ_ARR_AT(item, _ref, 1).ptr = lambda;
  object_type_check(err, env, TYPE_VECTOR); PL_CHECK;
  OBJ_ARR_AT(item, _ref, 2).ptr = env;
  OBJ_ARR_AT(item, _ref, 3).ptr = empty_stack;
  OBJ_ARR_AT(item, _ref, 4).ptr = pc;
  OBJ_ARR_AT(item, _ref, 5).ptr = prev_frame;

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
int object_tuple_frame_pc_is_finish(err_t **err, object_t *frame){
  object_t *lambda = NULL;
  object_t *code = NULL;
  object_int_value_t current_pc;
  int ret = 0;

  lambda = object_tuple_frame_get_lambda(err, frame); PL_CHECK;

  code = object_tuple_lambda_get_code(err, lambda); PL_CHECK;

  current_pc = object_tuple_frame_get_pc(err, frame)->part._int.value; PL_CHECK;

  ret = (size_t)current_pc >= object_array_count(err, code);

  PL_FUNC_END
  return ret;
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

err_t *object_tuple_frame_set_pc(err_t **err, object_t *frame, object_int_value_t new_pc){
  object_tuple_frame_get_pc(err, frame)->part._int.value = new_pc; PL_CHECK;
  PL_FUNC_END
  return *err;
}
err_t *object_tuple_frame_inc_pc(err_t **err, object_t *frame){
  object_tuple_frame_get_pc(err, frame)->part._int.value++;
  return *err;
}
// return key-value pair
object_t *object_tuple_env_resolve(err_t **err, gc_manager_t *gcm, object_t *env, object_t *symbol){
  size_t gcm_stack_depth;
  size_t i;
  object_t *key_value_pair = NULL;
  object_t *key = NULL;
  object_t *prev_cons = NULL;
  object_t *prev_env = NULL;

  if(env == NULL) {return NULL;}

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &prev_cons); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &prev_env); PL_CHECK;

  object_type_check(err, env, TYPE_VECTOR); PL_CHECK;
  for(i=1; i<env->part._vector.count; i++){
    key_value_pair = object_vector_ref_index(err, env, (int)i); PL_CHECK;
    key = object_tuple_cons_get_car(err, key_value_pair); PL_CHECK;
    
    object_type_check(err, key, TYPE_SYMBOL); PL_CHECK;
    object_type_check(err, symbol, TYPE_SYMBOL); PL_CHECK;
    
//     printf("[debug] test str ");
//     object_write(err, key); PL_CHECK;
//     printf(" == ");
//     object_write(err, symbol); PL_CHECK;
//     printf("\n");
    
    if(object_str_eq(key->part._symbol.name, symbol->part._symbol.name)){
      // found
      break;
    }
  }
  if(i >= env->part._vector.count){
    // if not found in current env
    prev_cons = object_vector_ref_index(err, env, 0);
    prev_env = object_tuple_cons_get_cdr(err, prev_cons); PL_CHECK;
    if(prev_env != NULL){
      key_value_pair = object_tuple_env_resolve(err, gcm, prev_env, symbol);
    }else{
      key_value_pair = NULL;
    }
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
  gc_manager_stack_object_push(err, gcm, &frame); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &value); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &key_value_pair); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &env); PL_CHECK;

  key_value_pair = object_tuple_cons_alloc(err, gcm, symbol, value); PL_CHECK;
  env = object_tuple_frame_get_env(err, frame); PL_CHECK;
  object_vector_ref_push(err, gcm, env, key_value_pair); PL_CHECK;

  PL_FUNC_END;
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}
