#include "include/scope.h"
#include "include/utils.h"
#include <stdio.h>
#include <string.h>

scope_T *init_scope()
{
    scope_T *scope = calloc(1, sizeof(struct SCOPE_STRUCT));
    scope->symbol_table = NULL;
    scope->function_definitions = NULL;

    return scope;
}

AST_T *scope_add_func_def(scope_T *scope, AST_T *func_def)
{
    if (scope->function_definitions == NULL)
        scope->function_definitions = init_hash_map();
    map_put(scope->function_definitions, func_def->func_name, func_def);
    return func_def;
}

AST_T *scope_get_func_def(scope_T *scope, const char *fname)
{
    return map_get(scope->function_definitions, fname);
}

AST_T *scope_add_var_def(scope_T *scope, AST_T *var_def)
{
    if (scope->symbol_table == NULL)
        scope->symbol_table = init_hash_map();
    map_put(scope->symbol_table, var_def->var_def_var_name, var_def);
    return var_def;
}

AST_T *scope_get_var_def(scope_T *scope, const char *vname)
{
    return map_get(scope->symbol_table, vname);
}

AST_T *scope_replace_var_def(scope_T *scope, AST_T *var_def)
{
    return scope_add_var_def(scope, var_def);
}

void scope_rem_var_def(scope_T *scope, const char *vname)
{
    map_remove(scope->symbol_table, vname);
}