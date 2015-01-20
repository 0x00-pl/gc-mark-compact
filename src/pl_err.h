#ifndef _PL_ERR_H_
#define _PL_ERR_H_

typedef int err_t;
#define ERR_NO_ERR        0
#define ERR_HANDLED       0
#define ERR_ANY          -1
#define ERR_NULL_PTR     -2
#define ERR_ALLOC        -3
#define ERR_TYPECHECK    -4
#define ERR_OUT_OF_RANGE -5

#define ERRLEVEL_NO_ERR 0
#define ERRLEVEL_WARING 1
#define ERRLEVEL_ERROR  2

// err_t err(err_t type, int level, const char *msg);
// 
// err_t err_null();
// 
// err_t err_alloc(const char *msg);
// 
// err_t err_typecheck(const char *needed, const char *given);
// 
// err_t err_out_of_range();

err_t err(const char* msg);
err_t err_null();
err_t err_alloc();
err_t err_typecheck();
err_t err_out_of_range();

// #define PL_ASSERT(exp, err_exp, err_next)\
  if(exp){}else{ \
    err_exp; \
    err_next; \
  }

// #define PL_CHECK(exp, err_exp, err_next) PL_ASSERT( ((exp)>=ERR_NO_ERR), err_exp, err_next )

// #define PL_NOT_NULL(exp) PL_ASSERT((exp)!=NULL, PL_PRINT_LOCATION, return ERR_NULL_PTR)

#define PL_PRINT_LOCATION printf("at file: %s line: %d \n\n", __FILE__, __LINE__)

#define PL_CHECK_RET_BEG int err = 0;

#  define PL_ASSERT(cond, err_exp) \
    if(cond){}else{ \
      err = (err_exp); \
      PL_PRINT_LOCATION; \
      goto fin; \
    }
#  define PL_ASSERT_EX(cond, err_exp, err_next) \
    if(cond){}else{ \
      err_exp; \
      PL_PRINT_LOCATION; \
      err_next; \
    }
    
#  define PL_ASSERT_NOT_NULL(exp) PL_ASSERT((exp)!=NULL, err_null())

#  define PL_CHECK_RET(exp) if((err = (exp)) >= 0) {} else {PL_PRINT_LOCATION; goto fin;}

#define PL_CHECK_RET_END fin: return err;
#define PL_CHECK_RET_END_EX(free_local, free_ret) \
  fin: \
  if(0){}else{ \
    free_local; \
    if(err<0){ \
      free_ret; \
    }else{} \
    return err; \
  }
  
#endif