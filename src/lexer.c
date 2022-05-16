#include "include/lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Initialize the lexer
lexer_T *init_lexer(char *contents)
{
    lexer_T *lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->index = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->c = contents[lexer->index];
    lexer->len = strlen(contents);
    return lexer;
}

void lexer_advance(lexer_T *lexer)
{
    if (lexer->c != '\0' && lexer->index < lexer->len)
    {
        if (lexer->c == 10)
        {
            lexer->line += 1;
            lexer->col = 0;
        }
        lexer->index += 1;
        lexer->col += 1;
        lexer->c = lexer->contents[lexer->index];
    }
}

void lexer_skip_whitespace(lexer_T *lexer)
{
    while (lexer->c == ' ' || lexer->c == 10) // skips spaces and new lines (the 10)
    {
        lexer_advance(lexer);
    }
}

/**
 * @brief Skips tokens between two '~' tokens. This acts like a block comment
 *
 * @param lexer An instance of a lexer struct
 */
void lexer_skip_block_comments(lexer_T *lexer)
{
    unsigned int start_line = lexer->line;
    unsigned int start_col = lexer->col;
    lexer_advance(lexer);

    while (lexer->c != '~' && lexer->index < lexer->len)
    {
        lexer_advance(lexer);
    }
    if (lexer->index == lexer->len)
    {
        printf("%d:%d -- Unclosed block comment. Use '~' on both sides to open and close a comment\n", start_line, start_col);
    }
    lexer_advance(lexer);
}

/**
 * @brief Skips tokens between a '#' and a new line. This acts like an inline comment.
 *
 * @param lexer An instance of a lexer struct
 */
void lexer_skip_comments(lexer_T *lexer)
{
    while (lexer->c != 10 && lexer->index < lexer->len)
    {
        lexer_advance(lexer);
    }
}

char lexer_peek(lexer_T *lexer)
{
    int peek_pos = lexer->index + 1;
    if (peek_pos > lexer->len)
        return '\0';
    else
        return lexer->contents[peek_pos];
}

token_T *lexer_get_next_token(lexer_T *lexer)
{
    while (lexer->c != '\0' && lexer->index < lexer->len)
    {
        if (lexer->c == '~')
            lexer_skip_block_comments(lexer);
        if (lexer->c == '#')
            lexer_skip_comments(lexer);

        if (lexer->c == ' ' || lexer->c == 10)
            lexer_skip_whitespace(lexer);

        if (isalpha(lexer->c))
            return lexer_collect_id(lexer);
        if (isdigit(lexer->c))
            return lexer_collect_number(lexer);
        switch (lexer->c)
        {
        case ';':
            return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '(':
            return lexer_advance_with_token(lexer, init_token(TOKEN_LPAR, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case ')':
            return lexer_advance_with_token(lexer, init_token(TOKEN_RPAR, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '{':
            return lexer_advance_with_token(lexer, init_token(TOKEN_LBRA, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '}':
            return lexer_advance_with_token(lexer, init_token(TOKEN_RBRA, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case ',':
            return lexer_advance_with_token(lexer, init_token(TOKEN_COMMA, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '+':
            return lexer_advance_with_token(lexer, init_token(TOKEN_PLUS, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '-':
            return lexer_advance_with_token(lexer, init_token(TOKEN_SUB, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '*':
            return lexer_advance_with_token(lexer, init_token(TOKEN_MUL, lexer_get_current_as_string(lexer), lexer->line, lexer->col));

        case '&':
        {
            if (lexer_peek(lexer) == '&')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_AND, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
            }
            else
            {
                printf("%d:%d -- Unexpected token: '%c' expected '&'\n", lexer->line, lexer->col, lexer->c);

                exit(1);
            }
        }
        case '|':
        {
            if (lexer_peek(lexer) == '|')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_OR, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
            }
            else
            {
                printf("%d:%d -- Unexpected token: '%c' expected '|'\n", lexer->line, lexer->col, lexer->c);
                exit(1);
            }
        }
        case '<':
        {
            if (lexer_peek(lexer) == '=')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_LTE_COMP, "<=", lexer->line, lexer->col));
            }
            else
                return lexer_advance_with_token(lexer, init_token(TOKEN_LT_COMP, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
        }
        case '!':
        {
            if (lexer_peek(lexer) == '=')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_NEQ_COMP, "!=", lexer->line, lexer->col));
            }
            else
            {
                printf("%d:%d -- Unexpected token: '%c' expected '=' after the '!'\n", lexer->line, lexer->col, lexer->c);
                exit(1);
            }
        }
        case '>':
        {
            if (lexer_peek(lexer) == '=')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_GTE_COMP, ">=", lexer->line, lexer->col));
            }
            else
                return lexer_advance_with_token(lexer, init_token(TOKEN_GT_COMP, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
        }
        case '=':
        {
            if (lexer_peek(lexer) == '=')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_EQ_COMP, "==", lexer->line, lexer->col));
            }
            else
                return lexer_advance_with_token(lexer, init_token(TOKEN_EQ, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
        }
        case '/':
        {
            if (lexer_peek(lexer) == '/')
            {
                lexer_advance(lexer);
                return lexer_advance_with_token(lexer, init_token(TOKEN_INT_DIV, "//", lexer->line, lexer->col));
            }
            else
            {
                return lexer_advance_with_token(lexer, init_token(TOKEN_DIV, lexer_get_current_as_string(lexer), lexer->line, lexer->col));
            }

            break;
        }
        case '"':
            return lexer_collect_string(lexer);
            break;
        }
    }
    return init_token(TOKEN_EOF, "\0", lexer->line, lexer->col);
}

token_T *lexer_collect_string(lexer_T *lexer)
{
    lexer_advance(lexer);

    char *val = calloc(1, sizeof(char));
    val[0] = '\0';
    unsigned int start_col = lexer->col;

    while (lexer->c != '"')
    {
        if (lexer->c == '\\' && lexer_peek(lexer) == 'n')
        {
            lexer_advance(lexer);
            lexer->c = '\n';
        }
        char *s = lexer_get_current_as_string(lexer);
        val = realloc(val, strlen(val) + strlen(s) + 1 * sizeof(char));
        strcat(val, s);
        lexer_advance(lexer);
        if (lexer->c == '\0')
        {
            printf("%d:%d -- Unexpected token: End of File. Expected '\"'\n", lexer->line, lexer->col);
            exit(1);
        }
    }

    lexer_advance(lexer);

    return init_token(TOKEN_STR, val, lexer->line, start_col);
}

token_T *lexer_collect_number(lexer_T *lexer)
{
    char *val = calloc(1, sizeof(char));
    val[0] = '\0';
    unsigned int start_col = lexer->col;
    while (isdigit(lexer->c))
    {
        char *s = lexer_get_current_as_string(lexer);
        val = realloc(val, strlen(val) + strlen(s) + 1 * sizeof(char));
        strcat(val, s);
        lexer_advance(lexer);
    }
    if (lexer->c == '.')
    {
        char *s = lexer_get_current_as_string(lexer);
        val = realloc(val, strlen(val) + strlen(s) + 1 * sizeof(char));
        strcat(val, s);
        lexer_advance(lexer);
        while (isdigit(lexer->c))
        {
            char *s = lexer_get_current_as_string(lexer);
            val = realloc(val, strlen(val) + strlen(s) + 1 * sizeof(char));
            strcat(val, s);
            lexer_advance(lexer);
        }
        return init_token(TOKEN_FLOAT, val, lexer->line, start_col);
    }
    else
    {
        return init_token(TOKEN_INT, val, lexer->line, start_col);
    }
}
token_T *lexer_collect_id(lexer_T *lexer)
{

    char *val = calloc(1, sizeof(char));
    val[0] = '\0';
    unsigned int start_col = lexer->col;

    while (isalnum(lexer->c) || lexer->c == '_')
    {
        char *s = lexer_get_current_as_string(lexer);
        val = realloc(val, strlen(val) + strlen(s) + 1 * sizeof(char));
        strcat(val, s);
        lexer_advance(lexer);
    }

    return init_token(TOKEN_ID, val, lexer->line, start_col);
}

token_T *lexer_advance_with_token(lexer_T *lexer, token_T *token)
{
    lexer_advance(lexer);
    return token;
}

char *lexer_get_current_as_string(lexer_T *lex)
{
    char *str = calloc(2, sizeof(char));
    str[0] = lex->c;
    str[1] = '\0';

    return str;
}