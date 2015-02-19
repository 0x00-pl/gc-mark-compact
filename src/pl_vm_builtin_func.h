#ifndef _PL_VM_BUILTIN_FUNC_H_
#define _PL_VM_BUILTIN_FUNC_H_

#include "pl_err.h"
#include "pl_type.h"
#include "pl_gc.h"


typedef err_t *(*vm_step_op_call_c_function_t)(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame,object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_make_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_eval(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_parser(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_define(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_resolve(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_newline(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_display(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_write(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_read(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_slice(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_car(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_cdr(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_cons(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_eq(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_add(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_sub(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_mul(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_div(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_remainder(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_and(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_or(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_l(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_r(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_le(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);
err_t *vm_step_op_call_re(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_begin(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *add_builtin_object(err_t **err, gc_manager_t *gcm, object_t* frame, const char *str_name, object_t *value);
err_t *add_builtin_func(err_t **err, gc_manager_t *gcm, object_t* frame, const char *str_name, vm_step_op_call_c_function_t builtin_func);

#endif
