#include "pl_parser.h"
#include "pl_type.h"
#include "pl_gc.h"
#include "pl_err.h"
#include "pl_op_code.h"
#include <string.h>
#include <stdlib.h>



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
  return c=='\v'|c==' '|c=='\n'|c=='\t'|c=='\r'|c==';';
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
  size_t p = *pos;
  int escape = 0;
  int done = 0;
  char *temp_str = NULL;
  object_t *str_ret;
  
  if(text[p]!='"'){return NULL;}else{p++;}
  
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
  string_unescape(err, &text[*pos], p-*pos, temp_str); PL_CHECK;
  str_ret = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  object_str_init(err, str_ret, temp_str); PL_CHECK;
  
  *pos = p;
  PL_FUNC_END_EX(free(temp_str), str_ret=NULL);
  return str_ret;
}
object_t *parser_parse_node_sharp_boolean(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  if(size!=2){return NULL;}
  if(text[0]!='#'){return NULL;}
  if(text[1]!='t' && text[1]!='f'){return NULL;}
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, text[1]=='f' ? 0 : 1); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_sharp_char(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  char c[2] = {'\0','\0'};
  if(size!=2){return NULL;}
  if(text[0]!='#'){return NULL;}
  if(text[1]!='\\'){return NULL;}
  
  ret = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  if(size == 3){
    c[0] = text[2];
  }else if(strncmp(text, "#\\space", size) == 0){
    c[0] = ' ';
  }else if(strncmp(text, "#\\newline", size) == 0){
    c[0] = '\n';
  }else{
    return NULL;
  }
  object_str_init(err, ret, c); PL_CHECK;
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_sharp_number(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  if(size<=2){return NULL;}
  if(text[0]!='#'){return NULL;}
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
      return NULL;
  }
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_sharp(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  if(text[0]!='#'){return NULL;}
  if((ret = parser_parse_node_sharp_boolean(err, gcm, text, size)) != NULL){
    return ret;
  }
  if((ret = parser_parse_node_sharp_char(err, gcm, text, size)) != NULL){
    return ret;
  }
  if((ret = parser_parse_node_sharp_number(err, gcm, text, size)) != NULL){
    return ret;
  }
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}

object_t *parser_parse_node_number_b(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  size_t int_value = 0;
  char str_buff[size+1];
    
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
        return NULL;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_number_o(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  size_t int_value = 0;
  char str_buff[size+1];
    
  //sign
  if(text[0]=='+') {sign = 1; p++;}
  else if(text[0]=='-') {sign = -1; p++;}
  else{sign = 1;}
  
  for(;p<size;p++){
    int_value = int_value * 8;
    if('0'<=text[p] && text[p]<='8'){
      int_value += text[p]-'0';
    }else{
      return NULL;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_number_d(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  size_t int_value = 0;
  size_t float_value = 0;
  size_t float_cursor = 1;
  char str_buff[size+1];
    
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
      return NULL;
    }
  }
  if(p>=size){
    ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
    object_int_init(err, ret, sign * int_value); PL_CHECK;
  }else{
    if(text[p]!='.'){return NULL;}
    else{p++;}
    
    float_value = int_value;
    for(;p<size;p++){
      float_cursor = float_cursor / 10;
      if('0'<=text[p] && text[p]<='9'){
        float_value += float_cursor * (text[p]-'0');
      }else{
        return NULL;
      }
    }
    ret = gc_manager_object_alloc(err, gcm, TYPE_FLOAT); PL_CHECK;
    object_float_init(err, ret, sign * float_value); PL_CHECK;
  }
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_number_x(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  size_t p = 0;
  int sign = 1;
  size_t int_value = 0;
  char str_buff[size+1];
    
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
      return NULL;
    }
  }
  ret = gc_manager_object_alloc(err, gcm, TYPE_INT); PL_CHECK;
  object_int_init(err, ret, sign * int_value); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}
object_t *parser_parse_node_symbol(err_t **err, gc_manager_t * gcm, const char *text, size_t size){
  object_t *ret = NULL;
  char symbol_str[size+1];
  
  strncpy(symbol_str, text, size);
  symbol_str[size] = '\0';
  ret = gc_manager_object_alloc(err, gcm, TYPE_STR); PL_CHECK;
  object_str_init(err, ret, symbol_str); PL_CHECK;
  
  PL_FUNC_END_EX(,ret=NULL)
  return ret;
}

object_t *parser_parse_node(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos){
  object_t *ret = NULL;
  size_t p;
  if(parser_is_space(err, text[*pos])){
    parser_skip_space(err, text, pos); PL_CHECK;
  }
  ret = parser_parse_string(err, gcm, text, pos); PL_CHECK;
  if(ret != NULL){
    return ret;
  }
  ret = parser_parse_exp(err, gcm, text, pos); PL_CHECK;
  if(ret != NULL){
    return ret;
  }
  
  p = *pos;
  while(!parser_is_space(err, text[p]) && text[p]!='(' && text[p]!=')' && text[p]!='"'){
    p++;
  }
  
  if(p-*pos == 1 && (text[*pos]=='+' || text[*pos]=='-')){
    ret = parser_parse_node_symbol(err, gcm, &text[*pos], p-*pos); PL_CHECK;
    if(ret != NULL){
      *pos = p;
      return ret;
    }
  }
  ret = parser_parse_node_sharp(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    return ret;
  }
  ret = parser_parse_node_number_d(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    return ret;
  }  
  ret = parser_parse_node_symbol(err, gcm, &text[*pos], p-*pos); PL_CHECK;
  if(ret != NULL){
    *pos = p;
    return ret;
  }
  
  PL_ASSERT(0, err_parsing);//unknow error
  PL_FUNC_END
  return NULL; //not match any atom-rule
}
object_t *parser_parse_exp(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos){
  object_t *ret = NULL;
  object_t *item = NULL;
  object_t *item_ref = NULL;
  size_t p;
  int is_vector = 0;
  
  if(parser_is_space(err, text[*pos])){
    parser_skip_space(err, text, pos); PL_CHECK;
  }
  
  p = *pos;
  if(text[p]=='#'){
    p++;
    is_vector = 1;
  }
  if(text[p]!='('){
    return NULL;
  }else{
    p++;
  }
  
  ret = gc_manager_object_alloc(err, gcm, TYPE_VECTOR); PL_CHECK;
  object_vector_init(err, ret); PL_CHECK;
  if(is_vector){
    item_ref = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_ref_init(err, item_ref, g_vector); PL_CHECK;
    object_vector_push(err, ret, gcm, item_ref);
  }
  
  while(1){
    if(parser_is_space(err, text[p])){
      parser_skip_space(err, text, &p); PL_CHECK;
    }
    
    if(text[p]==')'){ p++; break; }

    item = parser_parse_node(err, gcm, text, &p); PL_CHECK;
    if(item == NULL) {return NULL;}
    item_ref = gc_manager_object_alloc(err, gcm, TYPE_REF); PL_CHECK;
    object_ref_init(err, item_ref, item); PL_CHECK;
    object_vector_push(err, ret, gcm, item_ref);
  }
  
  ret = object_vector_to_array(err, ret, gcm);
  
  PL_FUNC_END_EX(,ret=NULL);
  return ret;
}

