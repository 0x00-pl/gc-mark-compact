/* ***this file is useless*** */
/* ***this file is useless*** */
/* ***this file is useless*** */

#include <stdio.h>
#include <stdlib.h>

#include "src/pl_gc.h"
#include "src/pl_parser.h"
#include "src/pl_op_code.h"
#include "src/pl_compile.h"
#include "src/pl_vm.h"
#include "src/pl_eval_file.h"
#include "test.h"


int main(int argc, char **argv){
  err_t *rerr = NULL;
  err_t **err = &rerr;
  gc_manager_t *gcm = NULL;
  
  // init global value
  op_init_global(err); PL_CHECK;
  gcm = malloc(sizeof(gc_manager_t));
  gc_manager_init(err, gcm); PL_CHECK;
  if(argc==2){
    eval_file(err, gcm, argv[1]);
  }else{
    test_main(err, gcm);
  }
  
  gc_manager_halt(err, gcm); PL_CHECK;
  
  op_free_global(err); PL_CHECK;
  PL_FUNC_END
  err_print(*err);
  return *err == 0 ? 0 : -1;
}
