#include "pl_eval_file.h"
#include "pl_err.h"
#include "pl_type.h"
#include "pl_vm.h"

#include <stdio.h>
#include <stdlib.h>


static size_t eval_file_file_size(err_t **err, FILE *ifile){
  (void)err;
  fseek(ifile, 0, SEEK_END);
  return (size_t)ftell(ifile);
}

static err_t *eval_file_load_file(err_t **err, const char *filename, char **pdest){
  size_t ifile_size;
  FILE *ifile = fopen(filename, "r");
  if(!ifile){
    printf("file not found : %s\n", filename);
    PL_ASSERT(0, err_io);
    return *err;
  }
  ifile_size = eval_file_file_size(err, ifile);
  *pdest = malloc(ifile_size+3);
  
  fseek(ifile, 0, SEEK_SET);
  fread((*pdest)+1, ifile_size, 1, ifile);
  
  (*pdest)[0] = '(';
  (*pdest)[ifile_size+1] = ')';
  (*pdest)[ifile_size+2] = '\0';
  
  PL_FUNC_END
  if(ifile){fclose(ifile);}
  return *err;
}


err_t *eval_file(err_t **err, gc_manager_t *gcm, const char *filename){
  char *text;
  
  eval_file_load_file(err, filename, &text);
  vm_eval_text(err, gcm, text); PL_CHECK;
  
  PL_FUNC_END;
  free(text);
  return *err;
}
