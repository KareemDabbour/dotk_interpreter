#ifndef PARSER_H
#define PARSER_H
#include "AST.h"
#include "lexer.h"
#include "scope.h"

typedef struct PARSER_STRUCT
{
    lexer_T *lexer;
    token_T *current_token;
    token_T *prev_token;
    scope_T *scope;
} parser_T;

parser_T *init_parser(lexer_T *lexer);

void parser_eat(parser_T *parser, int token_type);

AST_T *parser_parse(parser_T *parser, scope_T *scope);

AST_T *parser_parse_statements(parser_T *parser, scope_T *scope);

AST_T *parser_parse_expr(parser_T *parser, scope_T *scope, AST_T *parent);

AST_T *parser_parse_factor(parser_T *parser, scope_T *scope, AST_T *parent);

AST_T *parser_parse_term(parser_T *parser, scope_T *scope, AST_T *parent);

AST_T *parser_parse_func_call(parser_T *parser, scope_T *scope);

AST_T *parser_parse_func_def(parser_T *parser, scope_T *scope);

AST_T *parser_parse_var(parser_T *parser, scope_T *scope);

AST_T *parser_parse_var_def(parser_T *parser, scope_T *scope);

AST_T *parser_parse_str(parser_T *parser, scope_T *scope);

AST_T *parser_parse_id(parser_T *parser, scope_T *scope);

#endif