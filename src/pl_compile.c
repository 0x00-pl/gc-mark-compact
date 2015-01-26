#include "pl_compile.h"

#include <string.h>
#include "pl_parser.h"
#include "pl_op_code.h"

int parser_symbol_eq(object_t *symbol, const char *str){
  object_t *symbol_name = NULL;
  if(symbol->type != TYPE_SYMBOL) { return 0; }
  symbol_name = symbol->part._symbol.name;
  if(symbol_name->type != TYPE_STR) { return 0; }
  symbol_name = symbol->part._symbol.name;
  
  return strncmp(str, symbol_name->part._str.str, symbol_name->part._str.size) == 0;
}



object_t *compile_exp(err_t **err, gc_manager_t *gcm, object_t *exp, object_t *code_vector){
  object_t *args_count_obj = NULL;
  object_t *func_keyword = NULL;
  object_t *if_else_index = NULL;
  object_t *if_endif_index = NULL;
  object_t *lambda_object_lambda = NULL;
  size_t gcm_stack_depth;
  size_t args_count = 0;
  size_t i;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &code_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &func_keyword); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &args_count_obj); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &if_else_index); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &if_endif_index); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda_object_lambda); PL_CHECK;
  
  args_count_obj = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, args_count_obj, (object_int_value_t)-1); PL_CHECK;
      
  if(exp == NULL){
    object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
    object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_nil)); PL_CHECK;
    goto fin;
  }
  
  // exp is atom
  if(exp->type != TYPE_REF){
    // push sym(x)
    object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
    object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, exp)); PL_CHECK;
    
    if(exp->type == TYPE_SYMBOL){
      // if it is symbol
      // call 1
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_call)); PL_CHECK;
      object_int_init(err, args_count_obj, (object_int_value_t)1); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, args_count_obj)); PL_CHECK;
    }
  }else{
    args_count = object_array_count(err, exp); PL_CHECK;
    // if exp is ()
    if(args_count == 0){
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_nil)); PL_CHECK;
      goto fin;
    }
    
    // if func is keyword
    func_keyword = OBJ_ARR_AT(exp,_ref,0).ptr;
    if(parser_symbol_eq(func_keyword, "quote")){
      // push exp[1]
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      if(args_count == 1){
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_nil)); PL_CHECK;
      }else{
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr)); PL_CHECK;
      }
    }
    else if(parser_symbol_eq(func_keyword, "define")){
      if(args_count != 3){
        // bad syntax
        goto fin;
      }
      
      // push sym(define)
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_define)); PL_CHECK;
      
      // push exp[1]
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr)); PL_CHECK;
      
      // <exp[2]>
      compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
      
      // call 3
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_call)); PL_CHECK;
      args_count_obj = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, args_count_obj)); PL_CHECK;
    }
    else if(parser_symbol_eq(func_keyword, "if")){
      if(args_count == 3){
        // push sym(if)
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_if)); PL_CHECK;
        
        // <exp[1]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr, code_vector); PL_CHECK;
        
        // jn :endif
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_jn)); PL_CHECK;
        if_endif_index = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
        object_int_init(err, if_endif_index, -1); PL_CHECK;
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, if_endif_index)); PL_CHECK;
        
        // <exp[2]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;          
        
        // endif:
        object_int_init(err, if_endif_index, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
        
      }else if(args_count == 4){
        // push sym(if)
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_if)); PL_CHECK;
        
        // <exp[1]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr, code_vector); PL_CHECK;
        
        // jn :else
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_jn)); PL_CHECK;
        if_else_index = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
        object_int_init(err, if_else_index, -1); PL_CHECK;
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, if_else_index)); PL_CHECK;
        
        // <exp[2]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
        
        // jmp :endif
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_jmp)); PL_CHECK;
        if_endif_index = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
        object_int_init(err, if_endif_index, -1); PL_CHECK;
        object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, if_endif_index)); PL_CHECK;
        
        // else:
        object_int_init(err, if_else_index, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
        
        // <exp[3]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,3).ptr, code_vector); PL_CHECK;
        
        // endif:
        object_int_init(err, if_endif_index, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
      }else{
        // bad syntax
        goto fin;
      }
      
    }
    else if(parser_symbol_eq(func_keyword, "set!")){
      if(args_count != 3){
        // bad syntax
        goto fin;
      }
      
      // push sym(set)
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_set)); PL_CHECK;
      
      // push sym = exp[1]
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr)); PL_CHECK;
      
      // <exp[2]>
      compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
      
      // call 3
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_call)); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, args_count_obj)); PL_CHECK;
    }
    else if(parser_symbol_eq(func_keyword, "lambda")){
      if(args_count < 3){
        // bad syntax
        goto fin;
      }
      // push sym(lambda)
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, g_lambda)); PL_CHECK;
      
      // push args = exp[1]
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr)); PL_CHECK;
      
      // push code = <compile_lambda(exp)>
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_push)); PL_CHECK;
      lambda_object_lambda = compile_lambda(err, gcm, exp); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, lambda_object_lambda)); PL_CHECK;
      
      // call 3
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_call)); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, args_count_obj)); PL_CHECK;
    }
    else{
      // push args
      for(i=0; i<args_count; i++){
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,i).ptr, code_vector); PL_CHECK;
      }
      
      // call n
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_call)); PL_CHECK;
      object_int_init(err, args_count_obj, (object_int_value_t)args_count); PL_CHECK;
      object_vector_push(err, code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, args_count_obj)); PL_CHECK;
    }
  }
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return code_vector;
}

object_t *array_ref_symbol_2_array_symbol(err_t **err, gc_manager_t *gcm, object_t *array_ref_symbol){
  size_t gcm_stack_depth;
  size_t count;
  size_t i;
  object_t *array_symbol = NULL;
  
  object_type_check(err, array_ref_symbol, TYPE_REF); PL_CHECK;
  count = object_array_count(err, array_ref_symbol); PL_CHECK;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &array_ref_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &array_symbol); PL_CHECK;
  
  array_symbol = gc_manager_object_array_alloc(err, gcm, TYPE_SYMBOL, count);
  
  for(i=0; i<count; i++){
    object_type_check(err, OBJ_ARR_AT(array_ref_symbol,_ref,i).ptr, TYPE_SYMBOL); PL_CHECK;
    object_symbol_part_init(err, (&array_symbol->part._symbol)+i, OBJ_ARR_AT(array_ref_symbol,_ref,i).ptr->part._symbol.name); PL_CHECK;
  }
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return array_symbol;
}

object_t *compile_lambda(err_t **err, gc_manager_t *gcm, object_t *lambda_exp){
  size_t gcm_stack_depth;
  object_t *exp_code_vector = NULL;
  object_t *exp_code = NULL;
  object_t *lambda_argsname = NULL;
  object_t *lambda = NULL;
  
  object_type_check(err, lambda_exp, TYPE_REF); PL_CHECK;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &lambda_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &exp_code_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &exp_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda_argsname); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  
  exp_code_vector = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, exp_code_vector); PL_CHECK;
  
  compile_exp(err, gcm, OBJ_ARR_AT(lambda_exp,_ref,2).ptr, exp_code_vector); PL_CHECK;
  object_vector_push(err, exp_code_vector, gcm, gc_manager_object_alloc_ref(err, gcm, op_ret)); PL_CHECK;
  
  exp_code = object_vector_to_array(err, exp_code_vector, gcm);
  
  lambda_argsname = array_ref_symbol_2_array_symbol(err, gcm, OBJ_ARR_AT(lambda_exp,_ref,1).ptr); PL_CHECK;
  
  lambda = object_tuple_lambda_alloc(err, gcm, lambda_argsname, OBJ_ARR_AT(lambda_exp,_ref,2).ptr, exp_code, NULL); PL_CHECK;
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return lambda;
}

object_t *compile_global(err_t **err, gc_manager_t *gcm, object_t *exp){
  size_t gcm_stack_depth;
  object_t *code_vector = NULL;
  object_t *code_array = NULL;
  size_t i;
  size_t count;
  
  if(exp == NULL){ return NULL; }
  object_type_check(err, exp, TYPE_REF); PL_CHECK;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &code_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &code_array); PL_CHECK;
  
  code_vector = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, code_vector); PL_CHECK;
  
  count = object_array_count(err, exp); PL_CHECK;
  for(i=0; i<count; i++){
    compile_exp(err, gcm, OBJ_ARR_AT(exp, _ref, i).ptr, code_vector);
  }
  
  code_array = object_vector_to_array(err, code_vector, gcm);
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return code_array;
}

static size_t print_indentation(size_t indentation){
  while(indentation-->0){
    printf(" ");
  }
  return indentation;
}


err_t *compile_verbose_array(err_t **err, gc_manager_t *gcm, object_t *arr, size_t indentation){
  size_t gcm_stack_depth;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &arr); PL_CHECK;
  
  print_indentation(indentation);
  parser_verbose(err, arr); PL_CHECK;
  printf("\n");
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *compile_verbose_lambda(err_t **err, gc_manager_t *gcm, object_t *lambda, size_t indentation){
  size_t gcm_stack_depth;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  
  print_indentation(indentation);
  printf("(#lambda\n");
  
  compile_verbose_array(err, gcm, object_tuple_lambda_get_argname(err, lambda), indentation+1); PL_CHECK;
  
  print_indentation(indentation+1);
  parser_verbose(err, object_tuple_lambda_get_exp(err, lambda)); PL_CHECK;
  printf("\n");
  
  compile_verbose_code(err, gcm, object_tuple_lambda_get_code(err, lambda), indentation+1); PL_CHECK;
  
  print_indentation(indentation);
  printf(")\n");
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

err_t *compile_verbose_code(err_t **err, gc_manager_t *gcm, object_t *code, size_t indentation){
  size_t gcm_stack_depth;
  size_t count;
  size_t i;
  object_t *item = NULL;
  object_t *arg1 = NULL;
  
  if(code == NULL){
    print_indentation(indentation); printf("null");
    goto fin;
  }
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &arg1); PL_CHECK;
  
  count = object_array_count(err, code); PL_CHECK;
  for(i=0; i<count; i++){
    item = OBJ_ARR_AT(code,_ref,i).ptr;
    
    print_indentation(indentation);
    if(item == op_call){
      i++;
      arg1 = OBJ_ARR_AT(code,_ref,i).ptr;
      object_type_check(err, arg1, TYPE_INT); PL_CHECK;
      printf("call    : %ld\n", arg1->part._int.value);
    }else
    if(item == op_jmp){
      i++;
      arg1 = OBJ_ARR_AT(code,_ref,i).ptr;
      object_type_check(err, arg1, TYPE_INT); PL_CHECK;
      printf("jmp     : %ld\n", arg1->part._int.value);
    }else
    if(item == op_jn){
      i++;
      arg1 = OBJ_ARR_AT(code,_ref,i).ptr;
      object_type_check(err, arg1, TYPE_INT); PL_CHECK;
      printf("jn      : %ld\n", arg1->part._int.value);
    }else
    if(item == op_push){
      i++;
      printf("push    : ");
      arg1 = OBJ_ARR_AT(code,_ref,i).ptr;
      
      if(arg1 == g_if){
        printf("#if\n");
      }else if(arg1 == g_define){
        printf("#define\n");
      }else if(arg1 == g_lambda){
        printf("#lambda\n");
      }else if(arg1 == g_nil){
        printf("'()\n");
      }else if(arg1 == g_quote){
        printf("#quote\n");
      }else{
        // is not global value
        switch(arg1->type){
          case TYPE_INT:
            printf("%ld\n", arg1->part._int.value);
            break;
          case TYPE_FLOAT:
            printf("%lf\n", arg1->part._float.value);
            break;
          case TYPE_SYMBOL:
            arg1 = arg1->part._symbol.name;
            object_type_check(err, arg1, TYPE_STR); PL_CHECK;
            printf("$%s\n", arg1->part._str.str);
            break;
          case TYPE_STR:            
            printf("\"%s\"\n", arg1->part._str.str);
            break;
          case TYPE_REF:
            if(object_tuple_is_lambda(err, arg1)){
              // print lambda
              compile_verbose_lambda(err, gcm, arg1, indentation+1);
            }else{
              // print array
              compile_verbose_array(err, gcm, arg1, indentation+1);
            }
            break;
          default:
            printf("unknow : ");
            object_verbose(err, arg1, 999, indentation+1, 0); PL_CHECK;
            printf("\n");
            break;
        }
      }
    }else
    if(item == op_ret){
      printf("ret     :\n");
    }else{
      printf("unknow  : ");
      object_verbose(err, item, 999, indentation+1, 0); PL_CHECK;
      printf("\n");
    }
  }
    
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}