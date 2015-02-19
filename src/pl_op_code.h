#ifndef _PL_OP_CODE_H_
#define _PL_OP_CODE_H_

#include "pl_type.h"
#include "pl_err.h"
#include "pl_gc.h"

object_t *g_cons;
object_t *g_frame;
object_t *g_lambda;
object_t *g_mklmd;
object_t *g_nil;
object_t *g_true;
object_t *g_define;
object_t *g_if;
object_t *g_set;
object_t *g_quote;


object_t *op_call;
object_t *op_ret;
object_t *op_find;
object_t *op_jmp;
object_t *op_jn;
object_t *op_push;
object_t *op_pop;
err_t *op_init_global(err_t **err);
err_t *op_free_global(err_t **err);

int object_is_nil(err_t **err, object_t *obj);

object_t *object_tuple_member_index(err_t **err, object_t *tuple, size_t index);

struct gc_manager_t_decl;

object_t *object_tuple_cons_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *ar, object_t *dr);
object_t *object_tuple_cons_get_car(err_t **err, object_t *cons);
err_t *object_tuple_cons_set_car(err_t **err, object_t *cons, object_t *ar);
object_t *object_tuple_cons_get_cdr(err_t **err, object_t *cons);
err_t *object_tuple_cons_set_cdr(err_t **err, object_t *cons, object_t *dr);

// lambda:{op_lambda, arg_name:[symbol], exp:ref, code:[ref], envname:[symbol]}
object_t *object_tuple_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *argname, object_t *exp, object_t *code, object_t *envname);
object_t *object_tuple_empty_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *code);
object_t *object_tuple_lambda_copy(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda);
int object_tuple_is_lambda(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_argname(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_exp(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_code(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_envname(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_env(err_t **err, object_t *lambda);
err_t *object_tuple_lambda_set_env(err_t **err, object_t *lambda, object_t *env);


// env:vector(cons(g_nil,prev_env), cons(symbol,object_t))
// frame:{op_frame, lambda, env:env, stack:vector(ref), pc:int, prev_frame:frame()}
object_t *object_tuple_frame_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda, object_t *env, object_t *prev_frame);
object_t *object_tuple_frame_get_lambda(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_env(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_stack(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_pc(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_prev_frame(err_t **err, object_t *frame);
int object_tuple_frame_pc_is_finish(err_t **err, object_t *frame);

object_t *object_tuple_frame_get_code(err_t **err, object_t *frame, int offset);
object_t *object_tuple_frame_get_current_code(err_t **err, object_t *frame);
err_t *object_tuple_frame_set_pc(err_t **err, object_t *frame, long int new_pc);
err_t *object_tuple_frame_inc_pc(err_t **err, object_t *frame);
object_t *object_tuple_env_resolve(err_t **err, gc_manager_t *gcm, object_t *env, object_t *symbol); // return key-value pair
object_t *object_tuple_frame_resolve(err_t **err, gc_manager_t *gcm, object_t *frame, object_t *symbol); // return key-value pair
err_t *object_tuple_frame_add_env(err_t **err, gc_manager_t *gcm, object_t *frame, object_t *symbol, object_t *value);

#endif
