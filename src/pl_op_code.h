#ifndef _PL_OP_CODE_H_
#define _PL_OP_CODE_H_

#include "pl_type.h"
#include "pl_err.h"
#include "pl_gc.h"

object_t *g_add;
object_t *g_cond;
object_t *g_cons;
object_t *g_frame;
object_t *g_lambda;
object_t *g_vector;
object_t *g_nil;

object_t *op_call;
object_t *op_ret;
object_t *op_find;
object_t *op_jmp;
object_t *op_jn;
object_t *op_push;
err_t *op_init_global(err_t **err, gc_manager_t *gcm);

int object_is_nil(err_t **err, object_t *obj);

object_t *object_tuple_member_index(err_t **err, object_t *tuple, size_t index);

struct gc_manager_t_decl;

object_t *object_tuple_cons_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *ar, object_t *dr);
object_t *object_tuple_cons_get_car(err_t **err, object_t *cons);
object_t *object_tuple_cons_get_cdr(err_t **err, object_t *cons);

// lambda:{op_lambda, arg_name:[symbol], exp:ref, code:[ref], env:[cons(symbol,ref)]}
object_t *object_tuple_lambda_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *argname, object_t *exp, object_t *code, object_t *env);
object_t *object_tuple_lambda_get_argname(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_exp(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_code(err_t **err, object_t *lambda);
object_t *object_tuple_lambda_get_env(err_t **err, object_t *lambda);


// env:[cons(g_nil,prev_env), cons(symbol,object_t)]
// frame:{op_frame, lambda, env:env, stack:vector(ref), pc:int, prev_frame:frame()}
object_t *object_tuple_frame_alloc(err_t **err, struct gc_manager_t_decl *gcm, object_t *lambda, object_t *env, object_t *prev_frame);
object_t *object_tuple_frame_get_lambda(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_env(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_stack(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_pc(err_t **err, object_t *frame);
object_t *object_tuple_frame_get_prev_frame(err_t **err, object_t *frame);


object_ref_part_t *object_tuple_frame_get_current_code(err_t **err, object_t *frame);
err_t *object_tuple_frame_set_pc(err_t **err, object_t *frame, long int new_pc);
err_t *object_tuple_frame_inc_pc(err_t **err, object_t *frame);

#endif
