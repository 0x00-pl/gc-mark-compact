#ifndef _PL_EVAL_FILE_H_
#define _PL_EVAL_FILE_H_

#include "pl_err.h"
#include "pl_gc.h"


err_t *eval_file(err_t **err, gc_manager_t *gcm, const char *filename);

#endif
