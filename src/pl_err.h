#ifndef _PL_ERR_H_
#define _PL_ERR_H_

#include <stddef.h>

#define ERR_NO_ERR        0
#define ERR_HANDLED       0
#define ERR_ANY          -1
#define ERR_NULL_PTR     -2
#define ERR_ALLOC        -3
#define ERR_TYPECHECK    -4
#define ERR_OUT_OF_RANGE -5
#define ERR_PARSING      -6
#define ERR_TESTING      -7
#define ERR_IO           -8
#define ERR_TRACE        -9

#define ERRLEVEL_NO_ERR 0
#define ERRLEVEL_WARING 1
#define ERRLEVEL_ERROR  2

typedef struct err_t_decl{
  int code;
  struct err_t_decl *inner;
  const char *file;
  size_t line;
  const char *extra_message;
} err_t;

int err_print(err_t *err);
int err_free(err_t *err);

err_t *err_new(err_t **err, const char *file, size_t line, const char *extra_message, int code);
err_t *err_null(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_alloc(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_typecheck(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_out_of_range(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_parsing(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_io(err_t **err, const char *file, size_t line, const char *extra_message);
err_t *err_testing(err_t **err, const char *file, size_t line, const char *extra_message);



#define PL_ERR_DEFAULT_ARGS err,__FILE__,__LINE__,NULL
#define PL_ERR_DEFAULT_MSG_ARGS(msg) err,__FILE__,__LINE__,msg

#  define PL_ASSERT(cond, err_func) \
    if(cond){}else{ \
      err_func(PL_ERR_DEFAULT_ARGS); \
      goto fin; \
    }
#  define PL_ASSERT_EX(cond, err_exp, err_next) \
    if(cond){}else{ \
      err_exp; \
      err_next; \
    }
    
#  define PL_ASSERT_NOT_NULL(exp) PL_ASSERT(NULL!=exp, err_null)

#  define PL_CHECK if(*err == NULL) {} else {err_new(PL_ERR_DEFAULT_ARGS, ERR_TRACE); goto fin;}

#define PL_FUNC_END fin:
#define PL_FUNC_END_EX(free_local, free_ret) \
  fin: \
  free_local; \
  if(*err == NULL){}else{ \
    free_ret; \
  }
  
#endif