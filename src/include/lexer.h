#ifndef LEXER_H
#define LEXER_H
#include "token.h"
#ifndef KRED
#define KRED "\033[31m"
#endif

#ifndef KNRM
#define KNRM "\033[0m"
#endif
typedef struct LEXER_STRUCT
{
    char c;
    unsigned int index;
    unsigned int line;
    unsigned int col;
    char *contents;
    int len;
} lexer_T;

// Initialize the lexer
lexer_T *init_lexer(char *contents);

void lexer_advance(lexer_T *lexer);

void lexer_skip_whitespace(lexer_T *lexer);

token_T *lexer_get_next_token(lexer_T *lexer);

token_T *lexer_collect_string(lexer_T *lexer);

token_T *lexer_collect_number(lexer_T *lexer);

token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token);

token_T *lexer_collect_id(lexer_T *lexer);

char *lexer_get_current_as_string(lexer_T *lexer);

#endif