#ifndef _PL_VM_H_
#define _PL_VM_H_

#include "pl_gc.h"
#include "pl_type.h"
#include "pl_err.h"
#include "pl_op_code.h"


// vm:{top_frame:frame(), }
object_t *vm_alloc(err_t** err, gc_manager_t* gcm, object_t* code);

object_t *object_tuple_vm_get_top_frame(err_t **err, object_t *vm);

err_t *object_tuple_vm_set_top_frame(err_t **err, object_t *vm, object_t *top_frame);

int vm_step(err_t** err, object_t* vm, gc_manager_t* gcm);

err_t *vm_add_stdlib(err_t **err, gc_manager_t *gcm, object_t *vm);

err_t *vm_verbose_cur_code(err_t **err, gc_manager_t *gcm, object_t *vm);
err_t *vm_verbose_stack(err_t **err, gc_manager_t *gcm, object_t *vm);
err_t *vm_verbose_env(err_t **err, gc_manager_t *gcm, object_t *vm);
err_t *vm_verbose_frame(err_t **err, gc_manager_t *gcm, object_t *vm);
err_t *vm_verbose(err_t **err, gc_manager_t *gcm, object_t *vm);

#endif
