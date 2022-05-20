#ifndef TOKEN_H
#define TOKEN_H
typedef struct TOKEN_STRUCT
{
    enum
    {
        TOKEN_ID,       // 0
        TOKEN_EQ,       // 1
        TOKEN_STR,      // 2
        TOKEN_SEMI,     // 3
        TOKEN_LPAR,     // 4
        TOKEN_RPAR,     // 5
        TOKEN_LBRA,     // 6
        TOKEN_RBRA,     // 7
        TOKEN_COMMA,    // 8
        TOKEN_EOF,      // 9
        TOKEN_PLUS,     // 10
        TOKEN_MUL,      // 11
        TOKEN_INT,      // 12
        TOKEN_FLOAT,    // 13
        TOKEN_SUB,      // 14
        TOKEN_DIV,      // 15
        TOKEN_INT_DIV,  // 16
        TOKEN_EQ_COMP,  // 17
        TOKEN_NEQ_COMP, // 18
        TOKEN_LT_COMP,  // 19
        TOKEN_GT_COMP,  // 20
        TOKEN_LTE_COMP, // 21
        TOKEN_GTE_COMP, // 22
        TOKEN_AND,      // 23
        TOKEN_OR,       // 24
        TOKEN_RSBRA,    // 25
        TOKEN_LSBRA     // 26
    } type;
    char *value;
    unsigned int line;
    unsigned int col;
} token_T;
token_T *init_token(int type, char *value, unsigned int line, unsigned int col);
#endif