#include "pl_op_code.h"


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
      return float_part->value == 0 /*|| isnan(float_part->value) TODO */;

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
object_t *object_tuple_cons_get_cdr(err_t **err, object_t *cons){
  return object_tuple_member_index(err, cons, 1);
}

// lambda:{op_lambda, arg_name:[symbol], exp:ref, code:[ref], env:[cons(symbol,ref)]}
object_t *object_tuple_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *argname, object_t *exp, object_t *code, object_t *env){
  PL_ASSERT(argname->type == TYPE_SYMBOL, err_typecheck);
  PL_ASSERT(exp->type == TYPE_REF, err_typecheck);
  PL_ASSERT(env->type == TYPE_REF, err_typecheck);
  
  object_t *new_lambda = object_tuple_alloc(err, gcm, 5); PL_CHECK;
  object_member_set_value(err, new_lambda, 0, g_lambda); PL_CHECK;
  object_member_set_value(err, new_lambda, 1, argname); PL_CHECK;
  object_member_set_value(err, new_lambda, 2, exp); PL_CHECK;
  object_member_set_value(err, new_lambda, 3, code); PL_CHECK;
  object_member_set_value(err, new_lambda, 4, env); PL_CHECK;
  PL_FUNC_END_EX(,new_lambda=NULL);
  return new_lambda;
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
object_t *object_tuple_lambda_get_env(err_t **err, object_t *lambda){
  return object_tuple_member_index(err, lambda, 4);
}
// env:[cons(g_nil,prev_env), cons(symbol,object_t)]
// frame:{op_frame, lambda, env:env, stack:vector(ref), pc:int, prev_frame:frame()}
object_t *object_tuple_frame_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda, object_t *env, object_t *prev_frame){
  object_t *item = object_tuple_alloc(err, gcm, 6); PL_CHECK;
  
  object_t *empty_stack = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, empty_stack); PL_CHECK;
  
  object_t *pc = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, pc, 0); PL_CHECK;
  
  object_member_set_value(err, item, 0, g_frame); PL_CHECK;
  object_member_set_value(err, item, 1, lambda); PL_CHECK;
  object_member_set_value(err, item, 2, env); PL_CHECK;
  object_member_set_value(err, item, 3, empty_stack); PL_CHECK;
  object_member_set_value(err, item, 4, pc); PL_CHECK;
  object_member_set_value(err, item, 5, prev_frame); PL_CHECK;
  PL_FUNC_END_EX(,item=NULL);
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

object_ref_part_t *object_tuple_frame_get_current_code(err_t **err, object_t *frame){
  object_ref_part_t *frame_part = object_as_ref(err, frame); PL_CHECK;
  object_t *lambda = frame_part[1].ptr;
  
  object_ref_part_t *lambda_part = object_as_ref(err, lambda); PL_CHECK;
  object_t *code = frame_part[3].ptr;
  
  object_int_part_t *pc = object_member_int(err, frame, 3); PL_CHECK;
  
  object_ref_part_t *code_part = object_as_ref(err, code); PL_CHECK;
  
  return &code_part[pc->value];
  PL_FUNC_END
  return NULL;
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