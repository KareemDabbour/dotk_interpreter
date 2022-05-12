#include "include/AST.h"
#include <stdlib.h>

AST_T *init_ast(int type)
{
    AST_T *ast = calloc(1, sizeof(struct AST_STRUCT));

    ast->scope = (void *)0;
    ast->global_scope = (void *)0;
    ast->type = type;

    // AST VAR DEF
    ast->var_def_var_name = (void *)0;
    ast->var_def_expr = (void *)0;
    ast->var_def_val = (void *)0;

    // AST VAR
    ast->var_name = (void *)0;

    // AST FUNC CALL
    ast->func_call_name = (void *)0;
    ast->func_call_args = (void *)0;
    ast->func_call_args_size = 0;

    // AST FUNC DEF
    ast->func_body = (void *)0;
    ast->func_name = (void *)0;
    ast->func_def_args = (void *)0;
    ast->func_call_args_size = 0;

    // AST IF STATEMENT
    ast->if_body = (void *)0;
    ast->else_body = (void *)0;
    ast->if_predicate = (void *)0;

    // AST BOOL
    ast->is_true = 0;

    // AST STR
    ast->str_val = (void *)0;

    // AST INT
    ast->int_val = 0;

    // AST ARR
    ast->arr = (void *)0;
    ast->arr_len = 0;

    // AST FLOAT
    ast->float_val = 0;

    // AST RET STATEMENT
    ast->return_expr = (void *)0;

    // AST OP
    ast->op_left = (void *)0;
    ast->op_right = (void *)0;

    // AST COMPOUND
    ast->compound_val = (void *)0;
    ast->compound_size = 0;
    ast->parent = (void *)0;

    return ast;
}