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
  size_t if_else_index;
  size_t if_endif_index;
  object_t *lambda_object_lambda = NULL;
  size_t gcm_stack_depth;
  size_t args_count = 0;
  size_t i;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &code_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &func_keyword); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &args_count_obj); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda_object_lambda); PL_CHECK;
  
  args_count_obj = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, args_count_obj, (object_int_value_t)-1); PL_CHECK;
      
  if(exp == NULL){
    object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
    object_vector_ref_push(err, gcm, code_vector, g_nil); PL_CHECK;
    goto fin;
  }
  
  // exp is atom
  if(exp->type != TYPE_REF){
    // push sym(x)
    object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
    object_vector_ref_push(err, gcm, code_vector, exp); PL_CHECK;
    
    if(exp->type == TYPE_SYMBOL){
      // if it is symbol
      // call 1
      object_vector_ref_push(err, gcm, code_vector, op_call); PL_CHECK;
      object_int_init(err, args_count_obj, (object_int_value_t)1); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, args_count_obj); PL_CHECK;
    }
  }else{
    args_count = object_array_count(err, exp); PL_CHECK;
    // if exp is ()
    if(args_count == 0){
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, g_nil); PL_CHECK;
      goto fin;
    }
    
    // if func is keyword
    func_keyword = OBJ_ARR_AT(exp,_ref,0).ptr;
    if(parser_symbol_eq(func_keyword, "quote")){
      // push exp[1]
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      if(args_count == 1){
        object_vector_ref_push(err, gcm, code_vector, g_nil); PL_CHECK;
      }else{
        object_vector_ref_push(err, gcm, code_vector, OBJ_ARR_AT(exp,_ref,1).ptr); PL_CHECK;
      }
    }
    else if(parser_symbol_eq(func_keyword, "define")){
      if(args_count != 3){
        // bad syntax
        goto fin;
      }
      
      // push sym(define)
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, g_define); PL_CHECK;
                   
      // push exp[1]
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, OBJ_ARR_AT(exp,_ref,1).ptr); PL_CHECK;
      
      // <exp[2]>
      compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
      
      // call 3
      object_vector_ref_push(err, gcm, code_vector, op_call); PL_CHECK;
      args_count_obj = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, args_count_obj); PL_CHECK;
    }
    else if(parser_symbol_eq(func_keyword, "if")){
      if(args_count == 3){
        // push sym(if)
        object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
        object_vector_ref_push(err, gcm, code_vector, g_if); PL_CHECK;
        
        // <exp[1]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr, code_vector); PL_CHECK;
        
        // jn :endif
        object_vector_ref_push(err, gcm, code_vector, op_jn); PL_CHECK;
        if_endif_index = code_vector->part._vector.count;
        object_vector_ref_push(err, gcm, code_vector, gc_manager_object_alloc(err, gcm, TYPE_INT)); PL_CHECK;
        
        // <exp[2]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;          
        
        // endif:
        
        object_int_init(err, OBJ_ARR_AT(code_vector->part._vector.pdata, _ref, if_endif_index).ptr, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
        
      }else if(args_count == 4){
        // push sym(if)
        object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
        object_vector_ref_push(err, gcm, code_vector, g_if); PL_CHECK;
        
        // <exp[1]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,1).ptr, code_vector); PL_CHECK;
        
        // jn :else
        object_vector_ref_push(err, gcm, code_vector, op_jn); PL_CHECK;
        if_else_index = code_vector->part._vector.count;
        object_vector_ref_push(err, gcm, code_vector, gc_manager_object_alloc(err, gcm, TYPE_INT)); PL_CHECK;
        
        // <exp[2]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
        
        // jmp :endif
        object_vector_ref_push(err, gcm, code_vector, op_jmp); PL_CHECK;
        if_endif_index = code_vector->part._vector.count;
        object_vector_ref_push(err, gcm, code_vector, gc_manager_object_alloc(err, gcm, TYPE_INT)); PL_CHECK;
        
        // else:
        object_int_init(err, OBJ_ARR_AT(code_vector->part._vector.pdata, _ref, if_else_index).ptr, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
        
        // <exp[3]>
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,3).ptr, code_vector); PL_CHECK;
        
        // endif:
        object_int_init(err, OBJ_ARR_AT(code_vector->part._vector.pdata, _ref, if_endif_index).ptr, (object_int_value_t)code_vector->part._vector.count); PL_CHECK;
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
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, g_set); PL_CHECK;
      
      // push sym = exp[1]
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, OBJ_ARR_AT(exp,_ref,1).ptr); PL_CHECK;
      
      // <exp[2]>
      compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, code_vector); PL_CHECK;
      
      // call 3
      object_vector_ref_push(err, gcm, code_vector, op_call); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, args_count_obj); PL_CHECK;
    }
    else if(parser_symbol_eq(func_keyword, "lambda")){
      if(args_count < 3){
        // bad syntax
        goto fin;
      }
      // push sym(lambda)
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, g_mklmd); PL_CHECK;
      
      // push args = exp[1]
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, OBJ_ARR_AT(exp,_ref,1).ptr); PL_CHECK;
      
      // push code = <compile_lambda(exp)>
      object_vector_ref_push(err, gcm, code_vector, op_push); PL_CHECK;
      lambda_object_lambda = compile_lambda(err, gcm, exp); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, lambda_object_lambda); PL_CHECK;
      
      // call 3
      object_vector_ref_push(err, gcm, code_vector, op_call); PL_CHECK;
      object_int_init(err, args_count_obj, 3); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, args_count_obj); PL_CHECK;
    }
    else{
      // push args
      for(i=0; i<args_count; i++){
        compile_exp(err, gcm, OBJ_ARR_AT(exp,_ref,i).ptr, code_vector); PL_CHECK;
      }
      
      // call n
      object_vector_ref_push(err, gcm, code_vector, op_call); PL_CHECK;
      object_int_init(err, args_count_obj, (object_int_value_t)args_count); PL_CHECK;
      object_vector_ref_push(err, gcm, code_vector, args_count_obj); PL_CHECK;
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
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &array_ref_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &array_symbol); PL_CHECK;
  
  count = object_array_count(err, array_ref_symbol); PL_CHECK;
  if(count!=0) {object_type_check(err, array_ref_symbol, TYPE_REF); PL_CHECK;}
  
  array_symbol = gc_manager_object_array_alloc(err, gcm, TYPE_SYMBOL, count);
  
  for(i=0; i<count; i++){
    object_type_check(err, OBJ_ARR_AT(array_ref_symbol,_ref,i).ptr, TYPE_SYMBOL); PL_CHECK;
    object_symbol_init_nth(err, array_symbol, (int)i, OBJ_ARR_AT(array_ref_symbol,_ref,i).ptr->part._symbol.name); PL_CHECK;
  }
  
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return array_symbol;
}


err_t *compile_lambda_get_env(err_t **err, gc_manager_t *gcm, object_t *exp, object_t *unresolved_symbol_vector, object_t *resolved_symbol_vector){
  size_t gcm_stack_depth;
  size_t i;
  size_t count;
  int symbol_found;
  object_t *argnames = NULL;
  object_t *symbol = NULL;
  object_t *func_keyword = NULL;
  object_symbol_part_t *resolved_symbol_pdata = NULL;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &unresolved_symbol_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &resolved_symbol_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &argnames); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &func_keyword); PL_CHECK;
  
  if(exp->type == TYPE_REF){
    if(object_array_count(err, exp) == 0){
      // ignore ()
      goto fin;
    }
    
    func_keyword = OBJ_ARR_AT(exp,_ref,0).ptr;
    if(parser_symbol_eq(func_keyword, "qoute")){
      // ignore quoted exp
      goto fin;
    }else if(parser_symbol_eq(func_keyword, "lambda")){
      // is lambda exp
      argnames = OBJ_ARR_AT(exp,_ref,1).ptr; PL_CHECK;
      count = object_array_count(err, argnames); PL_CHECK;
      for(i=0; i<count; i++){
        // mark argname:exp[1][i] to resolved
        object_type_check(err, argnames, TYPE_REF); PL_CHECK;
        symbol = OBJ_ARR_AT(argnames,_ref,i).ptr;
        object_vector_push(err, gcm, resolved_symbol_vector, symbol); PL_CHECK;
      }
      // mark exp[2]
      compile_lambda_get_env(err, gcm, OBJ_ARR_AT(exp,_ref,2).ptr, unresolved_symbol_vector, resolved_symbol_vector); PL_CHECK;
    }else{
      count = object_array_count(err, exp); PL_CHECK;
      for(i=0; i<count; i++){
        compile_lambda_get_env(err, gcm, OBJ_ARR_AT(exp,_ref,i).ptr, unresolved_symbol_vector, resolved_symbol_vector); PL_CHECK;
      }
    }
  }else{
    if(exp->type == TYPE_SYMBOL){
      symbol_found = 0;
      // there is no alloc or resize. so resolved_symbol_pdata never be broken. it's safe here.
      resolved_symbol_pdata = &(resolved_symbol_vector->part._vector.pdata->part._symbol);
      for(i=0; i<resolved_symbol_vector->part._vector.count; i++){
        if(parser_symbol_eq(exp, resolved_symbol_pdata[i].name->part._str.str)){
          symbol_found = 1;
          break;
        }
      }
      if(!symbol_found){
        // symbol is unresolved
        object_vector_push(err, gcm, unresolved_symbol_vector, exp);
        object_vector_push(err, gcm, resolved_symbol_vector, exp);
      }else{
        // symbol has already resolved, ignore.
      }
    }else{
      // ignore not symbol atom
    }
  }
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return *err;
}

object_t *compile_lambda(err_t **err, gc_manager_t *gcm, object_t *lambda_exp){
  size_t gcm_stack_depth;
  object_t *exp_code_vector = NULL;
  object_t *exp_code = NULL;
  object_t *unresolved_env_symbol = NULL;
  object_t *unresolved_env_symbol_array = NULL;
  object_t *resolved_env_symbol = NULL;
  object_t *lambda_argsname = NULL;
  object_t *lambda = NULL;
  
  object_type_check(err, lambda_exp, TYPE_REF); PL_CHECK;
  
  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &lambda_exp); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &exp_code_vector); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &exp_code); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda_argsname); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &lambda); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &unresolved_env_symbol); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &unresolved_env_symbol_array); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &resolved_env_symbol); PL_CHECK;
  
  exp_code_vector = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, exp_code_vector); PL_CHECK;
  
  compile_exp(err, gcm, OBJ_ARR_AT(lambda_exp,_ref,2).ptr, exp_code_vector); PL_CHECK;
  object_vector_ref_push(err, gcm, exp_code_vector, op_ret); PL_CHECK;
  
  exp_code = object_vector_to_array(err, exp_code_vector, gcm);
  
  lambda_argsname = array_ref_symbol_2_array_symbol(err, gcm, OBJ_ARR_AT(lambda_exp,_ref,1).ptr); PL_CHECK;
  
  // get env_names
  resolved_env_symbol = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, resolved_env_symbol); PL_CHECK;
  unresolved_env_symbol = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, unresolved_env_symbol); PL_CHECK;
  compile_lambda_get_env(err, gcm, lambda_exp, unresolved_env_symbol, resolved_env_symbol); PL_CHECK;
  
  unresolved_env_symbol_array = object_vector_to_array(err, unresolved_env_symbol, gcm);
  
  lambda = object_tuple_lambda_alloc(err, gcm, lambda_argsname, OBJ_ARR_AT(lambda_exp,_ref,2).ptr, exp_code, unresolved_env_symbol_array); PL_CHECK;
  
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
  
  object_vector_ref_push(err, gcm, code_vector, op_ret); PL_CHECK;
  
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
  
  printf("(#lambda\n");
  
  compile_verbose_array(err, gcm, object_tuple_lambda_get_argname(err, lambda), indentation+1); PL_CHECK;
  
  print_indentation(indentation+1);
  parser_verbose(err, object_tuple_lambda_get_exp(err, lambda)); PL_CHECK;
  printf("\n");
  
  compile_verbose_array(err, gcm, object_tuple_lambda_get_envname(err, lambda), indentation+1); PL_CHECK;
  
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
      }else if(arg1 == g_mklmd){
        printf("#make-lambda\n");
      }else if(arg1 == g_nil){
        printf("'()\n");
      }else if(arg1 == g_quote){
        printf("#quote\n");
      }else if(object_array_count(err, arg1) == 0){
        printf("()\n");
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
              compile_verbose_array(err, gcm, arg1, 0);
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