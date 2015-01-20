#include "pl_err.h"
#include <stdio.h>

// err_t err(err_t type, int level, const char *msg){
//   if(level>ERRLEVEL_NO_ERR) {printf("error[%d]: %s\n", level, msg);}
//   return type;
// }
// 
// err_t err_null(){
//   return err(ERR_NULL_PTR, ERRLEVEL_ERROR, "NULL ptr.");
// }
// 
// err_t err_alloc(const char *msg){
//   printf("alloc: %s\n", msg);
//   return err(ERR_ALLOC, ERRLEVEL_ERROR, "alloc");
// }
// 
// err_t err_typecheck(const char *needed, const char *given){
//   printf("typecheck: needed <%s> but given <%s>\n", needed, given);
//   return err(ERR_TYPECHECK, ERRLEVEL_ERROR, "typecheck");
// }
// 
// err_t err_out_of_range(){
//   return err(ERR_OUT_OF_RANGE, ERRLEVEL_ERROR, "out of range.");
// }

err_t err(){
  return ERR_ANY;
}
err_t err_null(){
  err();
  return ERR_NULL_PTR;
}
err_t err_alloc(){
  err();
  return ERR_ALLOC;
}
err_t err_typecheck(){
  err();
  return ERR_TYPECHECK;
}
err_t err_out_of_range(){
  err();
  return ERR_OUT_OF_RANGE;
}