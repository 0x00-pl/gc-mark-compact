#ifndef _PL_VM_BUILTIN_FUNC_H_
#define _PL_VM_BUILTIN_FUNC_H_

#include "pl_err.h"
#include "pl_type.h"
#include "pl_gc.h"


typedef err_t *(*vm_step_op_call_c_function_t)(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame,object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_make_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_lambda(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

err_t *vm_step_op_call_define(err_t **err, gc_manager_t *gcm, object_t *vm, object_t* top_frame, object_t *func, object_t *stack, size_t args_count);

#endif
