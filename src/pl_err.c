#include "pl_err.h"
#include <stdio.h>
#include <stdlib.h>


int err_print(err_t *err){  
  if(err == NULL){
    printf("\n\n");
    return 0;
  }
  if(err->inner == NULL){
    printf("error: %d ", err->code);
  }
  if(err->extra_message != NULL){
    printf("msg: %s\n", err->code, err->extra_message);
  }
  
  if(err->file != NULL){
    printf("in file:%s line:%d \n", err->file, err->line);
  }
  err_print(err->inner);
  
  return 0;
}

int err_free(err_t *err){
  if(err == NULL){return 0;}
  err_free(err->inner);
  free(err->inner);
  return 0;
}
err_t *err_new(err_t **err, const char *file, size_t line, const char *extra_message, int code){
  err_t *new_err = (err_t*)malloc(sizeof(err_t));
  new_err->code = code;
  new_err->file = file;
  new_err->line = line;
  new_err->extra_message = extra_message;
  new_err->inner = *err;
  *err = new_err;
  return *err;
}
err_t *err_null(err_t **err, const char *file, size_t line, const char *extra_message){
  return err_new(err, file, line, extra_message, ERR_NULL_PTR);
}
err_t *err_alloc(err_t **err, const char *file, size_t line, const char *extra_message){
  return err_new(err, file, line, extra_message, ERR_ALLOC);
}
err_t *err_typecheck(err_t **err, const char *file, size_t line, const char *extra_message){
  return err_new(err, file, line, extra_message, ERR_TYPECHECK);
}
err_t *err_out_of_range(err_t **err, const char *file, size_t line, const char *extra_message){
  return err_new(err, file, line, extra_message, ERR_OUT_OF_RANGE);
}
