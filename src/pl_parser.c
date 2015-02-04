#include "pl_parser.h"
#include "pl_type.h"
#include "pl_gc.h"
#include "pl_err.h"
#include "pl_op_code.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



err_t *parser_skip_comment(err_t **err, const char *text, size_t *pos){
  size_t p = *pos;
  while(text[p++]!='\n'){}
  *pos = p;
  return *err;
}
err_t *parser_skip_space(err_t **err, const char *text, size_t *pos){
  size_t p = *pos;
  int done = 0;
  while(!done){
    switch(text[p]){
      case ';':
        parser_skip_comment(err, text, &p);
        break;
      case ' ':
      case '\n': case '\t': case '\r':
      case '\v':
        p++;
        break;
      default:
        done = 1;
    }
  }
  *pos = p;
  return *err;
}
int parser_is_space(err_t **err, char c){
  (void)err;
  return c=='\v'||c==' '||c=='\n'||c=='\t'||c=='\r'||c==';';
}
err_t *string_unescape(err_t **err, const char *src, size_t size, char *dst){
  size_t i;
  int escape = 0;
  for(i=0; i<size;){
    if(escape){
      escape = 0;
      switch(src[i]){
        case '\\':
          *dst++ = '\\';
          i++;
          break;
        case '"':
          *dst++ = '"';
          i++;
          break;
        default:
          *dst++ = '\\';
          *dst++ = src[i];
          i++;
          break;
      }
    }else switch(src[i]){
      case '\0':
        PL_ASSERT(0, err_parsing);
        break;
      case '\\':
        escape = 1;
        i++;
        break;
      default:
        *dst++ = src[i];
        i++;
        break;
    }
  }
  PL_FUNC_END
  *dst = '\0';
  return *err;
}
object_t *parser_parse_string(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos){
  size_t gcm_stack_depth;
  size_t p = *pos;
  int escape = 0;
  int done = 0;
  char *temp_str = NULL;
  object_t *str_ret = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &str_ret); PL_CHECK;

  if(text[p]!='"'){goto fin;}else{p++;}

  while(!done){
    if(escape){
      escape = 0;
      p++;
    }
    else switch(text[p]){
      case '\0':
        PL_ASSERT(0, err_parsing);
        break;
      case '"':
        p++;
        done = 1;
        break;
      case '\\':
        p++;
        escape = 1;
        break;
      default:
        p++;
        break;
    }
  }
  temp_str = (char*)malloc(p-*pos);
  // remove first '\"' and last '\"'
  string_unescape(err, &text[*pos]+1, p-*pos-2, temp_str); PL_CHECK;
  str_ret = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  object_str_init(err, str_ret, temp_str); PL_CHECK;

  *pos = p;
  PL_FUNC_END_EX(, str_ret=NULL)
  free(temp_str);
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return str_ret;
}
object_t *parser_parse_node_sharp_boolean(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  if(size!=2){goto fin;}
  if(text[0]!='#'){goto fin;}
  if(text[1]!='t' && text[1]!='f'){goto fin;}
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, text[1]=='f' ? 0 : 1); PL_CHECK;

  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_sharp_char(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  char c[2] = {'\0','\0'};
  if(size!=2){goto fin;}
  if(text[0]!='#'){goto fin;}
  if(text[1]!='\\'){goto fin;}

  ret = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  if(size == 3){
    c[0] = text[2];
  }else if(strncmp(text, "#\\space", size) == 0){
    c[0] = ' ';
  }else if(strncmp(text, "#\\newline", size) == 0){
    c[0] = '\n';
  }else{
    goto fin;
  }
  object_str_init(err, ret, c); PL_CHECK;
  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_sharp_number(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  if(size<=2){goto fin;}
  if(text[0]!='#'){goto fin;}
  switch(text[1]){
    case 'b':
      ret = parser_parse_node_number_b(err, gcm, text+2, size-2); PL_CHECK;
      break;
    case 'o':
      ret = parser_parse_node_number_o(err, gcm, text+2, size-2); PL_CHECK;
      break;
    case 'd':
      ret = parser_parse_node_number_d(err, gcm, text+2, size-2); PL_CHECK;
      break;
    case 'x':
      ret = parser_parse_node_number_x(err, gcm, text+2, size-2); PL_CHECK;
      break;
    default:
      goto fin;
  }
  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_sharp(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  if(text[0]!='#'){goto fin;}
  if((ret = parser_parse_node_sharp_boolean(err, gcm, text, size)) != NULL){
    goto fin;
  }
  if((ret = parser_parse_node_sharp_char(err, gcm, text, size)) != NULL){
    goto fin;
  }
  if((ret = parser_parse_node_sharp_number(err, gcm, text, size)) != NULL){
    goto fin;
  }
  return NULL; // no rule match
  PL_FUNC_END_EX(, ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}

object_t *parser_parse_node_number_b(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  object_int_value_t int_value = 0;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  //sign
  if(text[0]=='+') {sign = 1; p++;}
  else if(text[0]=='-') {sign = -1; p++;}
  else{sign = 1;}

  for(;p<size;p++){
    int_value = int_value * 2;
    switch(text[p]){
      case '0':
        int_value += 0;
        break;
      case '1':
        int_value += 1;
        break;
      default:
        ret=NULL;
        goto fin;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;

  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_number_o(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  object_int_value_t int_value = 0;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  //sign
  if(text[0]=='+') {sign = 1; p++;}
  else if(text[0]=='-') {sign = -1; p++;}
  else{sign = 1;}

  for(;p<size;p++){
    int_value = int_value * 8;
    if('0'<=text[p] && text[p]<='8'){
      int_value += text[p]-'0';
    }else{
      ret=NULL;
      goto fin;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;

  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_number_d(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  long int int_value = 0;
  double float_value;
  double float_cursor = 1;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  //sign
  if(text[0]=='+') {sign = 1; p++;}
  else if(text[0]=='-') {sign = -1; p++;}
  else{sign = 1;}

  for(;p<size;p++){
    int_value = int_value * 10;
    if('0'<=text[p] && text[p]<='9'){
      int_value += text[p]-'0';
    }else if(text[p]=='.'){
      break;
    }else{
      ret=NULL;
      goto fin;
    }
  }
  if(p>=size){
    ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
    object_int_init(err, ret, sign * int_value); PL_CHECK;
  }else{
    if(text[p]!='.'){
      ret=NULL;
      goto fin;
    }else{
      p++;
    }

    float_value = 1.0f * (double)int_value;
    for(;p<size;p++){
      float_cursor = float_cursor / 10;
      if('0'<=text[p] && text[p]<='9'){
        float_value += float_cursor * (text[p]-'0');
      }else{
        ret=NULL;
        goto fin;
      }
    }
    ret = gc_manager_object_alloc(err, gcm, TYPE_FLOAT); PL_CHECK;
    object_float_init(err, ret, sign * float_value); PL_CHECK;
  }
  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_number_x(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gcm_stack_depth;
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  object_int_value_t int_value = 0;

  gcm_stack_depth = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  //sign
  if(text[0]=='+') {sign = 1; p++;}
  else if(text[0]=='-') {sign = -1; p++;}
  else{sign = 1;}

  for(;p<size;p++){
    int_value = int_value * 16;
    if('0'<=text[p] && text[p]<='9'){
      int_value += text[p]-'0';
    }else if('a'<=text[p] && text[p]<='f'){
      int_value += 9 + text[p]-'a';
    }else{
      ret = NULL;
      goto fin;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;

  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gcm_stack_depth);
  return ret;
}
object_t *parser_parse_node_symbol(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  size_t gc_stack_size;
  object_t *name = NULL;
  object_t *ret = NULL;
  char symbol_str[size+1];

  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &name); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  strncpy(symbol_str, text, size);
  symbol_str[size] = '\0';

  name = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  object_str_init(err, name, symbol_str); PL_CHECK;
  ret = gc_manager_object_alloc(err, gcm, TYPE_SYMBOL); PL_CHECK;
  object_symbol_init(err, ret, name); PL_CHECK;

  PL_FUNC_END_EX(,ret=NULL)
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return ret;
}

object_t *parser_parse_node(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos){
  size_t gc_stack_size;
  object_t *ret = NULL;
  size_t p;

  gc_stack_size = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;

  if(parser_is_space(err, text[*pos])){
    parser_skip_space(err, text, pos); PL_CHECK;
  }
  ret = parser_parse_string(err, gcm, text, pos); PL_CHECK;
  if(ret != NULL){
    goto fin;
  }
  ret = parser_parse_exp(err, gcm, text, pos); PL_CHECK;
  if(ret != NULL){
    goto fin;
  }

  p = *pos;
  while(!parser_is_space(err, text[p]) && text[p]!='(' && text[p]!=')' && text[p]!='"'){
    p++;
  }

  if(p-*pos == 1 && (text[*pos]=='+' || text[*pos]=='-' || text[*pos]=='.')){
    ret = parser_parse_node_symbol(err, gcm, &text[*pos], p-*pos); PL_CHECK;
    if(ret != NULL){
      *pos = p;
      goto fin;
    }
  }
  ret = parser_parse_node_sharp(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    goto fin;
  }
  ret = parser_parse_node_number_d(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    goto fin;
  }
  ret = parser_parse_node_symbol(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    goto fin;
  }

  PL_ASSERT(0, err_parsing);//unknow error //not match any atom-rule
  PL_FUNC_END
  gc_manager_stack_object_balance(gcm, gc_stack_size);
  return ret;
}
object_t *parser_parse_exp(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos){
  object_t *ret = NULL;
  object_t *item = NULL;
  object_t *item_ref = NULL;
  size_t p;
  size_t count;
  int is_vector = 0;
  (void)count;

  size_t gs = gc_manager_stack_object_get_depth(gcm);
  gc_manager_stack_object_push(err, gcm, &ret); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item); PL_CHECK;
  gc_manager_stack_object_push(err, gcm, &item_ref); PL_CHECK;

  if(parser_is_space(err, text[*pos])){
    parser_skip_space(err, text, pos); PL_CHECK;
  }

  p = *pos;
  if(text[p]=='#'){
    p++;
    is_vector = 1;
  }
  if(text[p]!='('){
    goto fin;
  }else{
    p++;
  }

  ret = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, ret); PL_CHECK;
  if(is_vector){
    item = parser_parse_node_symbol(err, gcm, "vector", 6); PL_CHECK;
    if(item == NULL) {ret = NULL; goto fin;}
    item_ref = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_ref_init(err, item_ref, item); PL_CHECK;
    object_vector_push(err, gcm, ret, item_ref); PL_CHECK;
  }

  while(1){
    if(parser_is_space(err, text[p])){
      parser_skip_space(err, text, &p); PL_CHECK;
    }

    if(text[p]==')'){ p++; break; }

    item = parser_parse_node(err, gcm, text, &p); PL_CHECK;
    if(item == NULL) {ret = NULL; goto fin;}
    item_ref = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_ref_init(err, item_ref, item); PL_CHECK;
    object_vector_push(err, gcm, ret, item_ref); PL_CHECK;
  }

  count = ret->part._vector.count;
  ret = object_vector_to_array(err, ret, gcm); PL_CHECK;

  *pos = p;
  PL_FUNC_END_EX(gc_manager_stack_object_balance(gcm,gs), ret=NULL);
  return ret;
}

err_t *parser_verbose(err_t **err, object_t *exp){
  size_t count;
  size_t i,j;
  object_t *str = NULL;

  if(exp == NULL) {goto fin;}

  count = object_array_count(err, exp); PL_CHECK;
  if(count == 0) {printf("()"); goto fin;}
  switch(exp->type){
    case TYPE_REF:
      printf("(");
      for(i=0; i<count; i++){
        parser_verbose(err, OBJ_ARR_AT(exp, _ref, i).ptr); PL_CHECK;
        printf(" ");
      }
      printf(")");
      break;
    case TYPE_STR:
      for(i=0; i<count; i++){
        printf("\"%s\" ", OBJ_ARR_AT(exp, _str, i).str);
      }
      break;
    case TYPE_SYMBOL:
      for(i=0; i<count; i++){
        str = OBJ_ARR_AT(exp, _symbol, i).name;
        PL_ASSERT_NOT_NULL(str);
        object_type_check(err, str, TYPE_STR); PL_CHECK;
        printf("$%s ", str->part._str.str);
      }
      break;
    case TYPE_INT:
      for(i=0; i<count; i++){
        printf("%ld ", OBJ_ARR_AT(exp, _int, i).value);
      }
      break;
    case TYPE_VECTOR:
      for(i=0; i<count; i++){
        printf("#(");
        for(j=0; j<OBJ_ARR_AT(exp, _vector, i).count; j++){
          parser_verbose(err, OBJ_ARR_AT(OBJ_ARR_AT(exp, _vector, i).pdata, _ref, j).ptr); PL_CHECK;
          printf(" ");
        }
        printf(") ");
      }
      break;
    default:
      object_verbose(err, exp, 999, 0, 0); PL_CHECK;
  }

  PL_FUNC_END
  return *err;
}


