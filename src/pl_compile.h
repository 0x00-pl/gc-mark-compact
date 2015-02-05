#ifndef _PL_COMPILE_H_
#define _PL_COMPILE_H_

#include "pl_type.h"
#include "pl_gc.h"


int parser_symbol_eq(object_t *symbol, const char *str);

object_t *compile_exp(err_t **err, gc_manager_t *gcm, object_t *exp, object_t *code_vector);

object_t *array_ref_symbol_2_array_symbol(err_t **err, gc_manager_t *gcm, object_t *array_ref_symbol);

object_t *compile_lambda(err_t **err, gc_manager_t *gcm, object_t *lambda_exp);

err_t *compile_quote(err_t **err, gc_manager_t *gcm, object_t *quoted_exp, object_t *code_vector);

object_t *compile_global(err_t **err, gc_manager_t *gcm, object_t *exp);


err_t *compile_verbose_array(err_t **err, gc_manager_t *gcm, object_t *arr, size_t indentation);

err_t *compile_verbose_lambda(err_t **err, gc_manager_t *gcm, object_t *lambda, size_t indentation);

err_t *compile_verbose_code(err_t **err, gc_manager_t *gcm, object_t *code, size_t indentation);

#endif
