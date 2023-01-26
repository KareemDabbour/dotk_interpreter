#ifndef SCOPE_H
#define SCOPE_H
#include "AST.h"
#include "HashMap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SCOPE_STRUCT
{
    HashMap_T *function_definitions;
    HashMap_T *symbol_table;
} scope_T;

scope_T *init_scope();

AST_T *scope_add_func_def(scope_T *scope, AST_T *func_def);

AST_T *scope_get_func_def(scope_T *scope, const char *fname);

AST_T *scope_add_var_def(scope_T *scope, AST_T *var_def);

AST_T *scope_replace_var_def(scope_T *scope, AST_T *var_def);

AST_T *scope_get_var_def(scope_T *scope, const char *vname);

void scope_rem_var_def(scope_T *scope, const char *vname);

#endif