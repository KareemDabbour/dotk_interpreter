#ifndef AST_H
#include <stdlib.h>
#define AST_H
#define MAX_STR_CHAR 2000
#define MAX_NUM_SPACE 20
typedef struct AST_STRUCT
{
    enum
    {
        AST_NOOP,                 // 0
        AST_FUNC_CALL,            // 1
        AST_FUNC_DEF,             // 2
        AST_VAR_DEF,              // 3
        AST_VAR,                  // 4
        AST_STR,                  // 5
        AST_COMPOUND,             // 6
        AST_INT,                  // 7
        AST_MUL,                  // 8
        AST_ADD,                  // 9
        AST_SUB,                  // 10
        AST_DIV,                  // 11
        AST_INT_DIV,              // 12
        AST_FLOAT,                // 13
        AST_IF_STMNT,             // 14
        AST_EQ_COMP,              // 15
        AST_LT_COMP,              // 16
        AST_GT_COMP,              // 17
        AST_LTE_COMP,             // 18
        AST_GTE_COMP,             // 19
        AST_AND,                  // 20
        AST_OR,                   // 21
        AST_BOOL,                 // 22
        AST_RET_STMNT,            // 23
        AST_NEQ_COMP,             // 24
        AST_ARR,                  // 25
        AST_ARR_DEF,              // 26
        AST_VAR_REDEF,            // 27
        AST_ARR_INDEX_ASSIGNMENT, // 28
        AST_ARR_INDEX             // 29
    } type;

    struct SCOPE_STRUCT *scope;
    struct SCOPE_STRUCT *global_scope;

    struct AST_STRUCT *parent;

    // AST VAR DEF & VAR REDEF
    char *var_def_var_name;
    struct AST_STRUCT *var_def_val;
    struct AST_STRUCT *var_def_expr;

    // AST VAR
    char *var_name;

    // AST FUNC CALL
    char *func_call_name;
    struct AST_STRUCT **func_call_args;
    size_t func_call_args_size;

    // AST FUNC DEF
    struct AST_STRUCT *func_body;
    char *func_name;
    struct AST_STRUCT **func_def_args;
    size_t func_def_args_size;

    // AST STR
    char *str_val;

    // AST INT
    int int_val;

    // AST INT
    float float_val;

    // AST MUL/ADD/DIV/INT DIV
    // AST EQ/LT/GT/LTE/GTE/NEQ
    struct AST_STRUCT *op_left;
    struct AST_STRUCT *op_right;

    // AST COMPOUND
    struct AST_STRUCT **compound_val;
    size_t compound_size;

    // AST IF STATEMENT
    struct AST_STRUCT *if_body;
    struct AST_STRUCT *else_body;
    struct AST_STRUCT *if_predicate;

    // AST RET STATEMENT
    struct AST_STRUCT *return_expr;

    // AST ARR
    struct AST_STRUCT **arr;
    size_t arr_size;

    // AST ARR DEF
    struct AST_STRUCT *arr_size_expr;

    // AST ARR INDEX
    struct AST_STRUCT *arr_index;
    struct AST_STRUCT *arr_new_val;

    struct AST_STRUCT *arr_inner_index;

    // AST BOOL
    int is_true;

    // Line and Column values
    unsigned int line;
    unsigned int col;

} AST_T;

AST_T *init_ast(int type, unsigned int line, unsigned int col);

AST_T *ast_visit(AST_T *node);

#endif