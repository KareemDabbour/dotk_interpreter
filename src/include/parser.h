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

AST_T *parser_parse(parser_T *parser, scope_T *scope);

#endif