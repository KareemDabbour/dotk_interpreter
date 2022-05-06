#include "include/scope.h"
#include <string.h>
#include <stdio.h>

scope_T *init_scope()
{
    scope_T *scope = calloc(1, sizeof(struct SCOPE_STRUCT));
    scope->function_definitions = (void *)0;
    scope->function_definitions_size = 0;
    scope->variable_definitions = (void *)0;
    scope->variable_definitions_size = 0;

    return scope;
}

AST_T *scope_add_func_def(scope_T *scope, AST_T *func_def)
{
    scope->function_definitions_size += 1;
    if (scope->function_definitions == (void *)0)
    {
        scope->function_definitions = calloc(1, sizeof(struct AST_STRUCT *));
    }
    else
    {
        scope->function_definitions = realloc(
            scope->function_definitions,
            scope->function_definitions_size * sizeof(struct AST_STRUCT *));
    }

    scope->function_definitions[scope->function_definitions_size - 1] = func_def;
    return func_def;
}

AST_T *scope_get_func_def(scope_T *scope, const char *fname)
{
    for (int i = 0; i < scope->function_definitions_size; i++)
    {
        AST_T *func_def = scope->function_definitions[i];

        if (strcmp(func_def->func_name, fname) == 0)
        {
            return func_def;
        }
    }
    return init_ast(AST_NOOP);
}

AST_T *scope_add_var_def(scope_T *scope, AST_T *var_def)
{
    // if (scope_get_var_def(scope, var_def->var_def_var_name)->type == AST_NOOP)
    // {
    //     if (scope->variable_definitions == (void *)0)
    //     {
    //         scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT *));
    //         scope->variable_definitions[0] = var_def;
    //         scope->variable_definitions_size += 1;
    //     }
    //     else
    //     {
    //         scope->variable_definitions_size += 1;
    //         scope->variable_definitions = realloc(
    //             scope->variable_definitions,
    //             scope->variable_definitions_size * sizeof(struct AST_STRUCT *));
    //         scope->variable_definitions[scope->variable_definitions_size - 1] = var_def;
    //     }
    // }
    // else
    // {
    //     for (int i = 0; i < scope->variable_definitions_size; i++)
    //     {

    //         if (strcmp(scope->variable_definitions[i]->var_def_var_name, var_def->var_def_var_name) == 0)
    //         {
    //             scope->variable_definitions[i]->var_def_val = var_def->var_def_val;
    //             break;
    //         }
    //     }
    // }
    if (scope->variable_definitions == (void *)0)
    {
        scope->variable_definitions = calloc(1, sizeof(struct AST_STRUCT *));
        scope->variable_definitions[0] = var_def;
        scope->variable_definitions_size += 1;
    }
    else
    {
        scope->variable_definitions_size += 1;
        scope->variable_definitions = realloc(
            scope->variable_definitions,
            scope->variable_definitions_size * sizeof(struct AST_STRUCT *));
        scope->variable_definitions[scope->variable_definitions_size - 1] = var_def;
    }
    return var_def;
}

AST_T *scope_get_var_def(scope_T *scope, const char *vname)
{

    for (int i = scope->variable_definitions_size - 1; i > -1; i--)
    {
        AST_T *vdef = scope->variable_definitions[i];
        if (strcmp(vdef->var_def_var_name, vname) == 0)
        {
            return vdef;
        }
    }
    return init_ast(AST_NOOP);
}