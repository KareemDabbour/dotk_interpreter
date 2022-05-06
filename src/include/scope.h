#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"

typedef struct SCOPE_STRUCT
{
    AST_T **function_definitions;
    size_t function_definitions_size;
    AST_T **variable_definitions;
    size_t variable_definitions_size;
} scope_T;

scope_T *init_scope();

AST_T *scope_add_func_def(scope_T *scope, AST_T *func_def);

AST_T *scope_get_func_def(scope_T *scope, const char *fname);

AST_T *scope_add_var_def(scope_T *scope, AST_T *var_def);

AST_T *scope_get_var_def(scope_T *scope, const char *vname);
#endif