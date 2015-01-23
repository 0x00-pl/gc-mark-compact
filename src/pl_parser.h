#ifndef _PL_PARSER_H_
#define _PL_PARSER_H_

#include "pl_type.h"
#include "pl_gc.h"
#include "pl_err.h"



err_t *parser_skip_comment(err_t **err, const char *text, size_t *pos);
err_t *parser_skip_space(err_t **err, const char *text, size_t *pos);
int parser_is_space(err_t **err, char c);
err_t *string_unescape(err_t **err, const char *src, size_t size, char *dst);
object_t *parser_parse_string(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos);
object_t *parser_parse_node_sharp_boolean(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_sharp_char(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_sharp_number(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_sharp(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_number_b(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_number_o(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_number_d(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_number_x(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node_symbol(err_t **err, gc_manager_t * gcm, const char *text, size_t size);
object_t *parser_parse_node(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos);
object_t *parser_parse_exp(err_t **err, gc_manager_t * gcm, const char *text, size_t *pos);



#endif

