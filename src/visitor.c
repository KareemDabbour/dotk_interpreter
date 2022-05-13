#include "include/visitor.h"
#include "include/scope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static AST_T *builtin_function_print(visitor_T *visitor, AST_T *node, AST_T **args, size_t args_size);
static AST_T *__visit_ret_stmnt__(visitor_T *visitor, AST_T *node, AST_T *to_visit);
static AST_T *visitor_visit_func_call(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_func_def(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_var_def(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_var(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_str(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_compound(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_int(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_bool(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_float(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_add(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_sub(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_mul(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_div(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_int_div(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_if_statement(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_ret_statement(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_and(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_or(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_not_eq_comp(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_eq_comp(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_lt_comp(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_gt_comp(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_lte_comp(visitor_T *visitor, AST_T *node);
static AST_T *visitor_visit_gte_comp(visitor_T *visitor, AST_T *node);

const char *enum_names[25] = {
    "NULL",
    "FUNCTION CALL",
    "FUNCTION DEFINITION",
    "VARIABLE DEFINITION",
    "VAR",
    "STR",
    "COMPOUND",
    "INT",
    "MULTIPLICATION",
    "ADDITION",
    "SUBTRACTION",
    "DIVISION",
    "INT DIVISION",
    "FLOAT",
    "IF STATEMENT",
    "EQUALS COMPARATOR",
    "LESS THAN COMPARATOR",
    "GREATER THAN COMPARATOR",
    "LESS THAN OR EQUAL COMPARATOR",
    "GREATER THAN OR EQUAL COMPARATOR",
    "AND COMPARATOR",
    "OR COMPARATOR",
    "BOOLEAN",
    "RETURN STATEMENT",
    "NOT EQUALS COMPARATOR",
};

AST_T *builtin_function_print(visitor_T *visitor, AST_T *node, AST_T **args, size_t args_size)
{
    for (size_t i = 0; i < args_size; i++)
    {
        AST_T *visited_ast = visitor_visit(visitor, args[i]);

        switch (visited_ast->type)
        {
        case AST_STR:
            printf("%s\n", visited_ast->str_val);
            break;
        case AST_INT:
            printf("%d\n", visited_ast->int_val);
            break;
        case AST_FLOAT:
            printf("%.2f\n", visited_ast->float_val);
            break;
        case AST_NOOP:
            printf("NULL\n");
            break;
        case AST_BOOL:
        {
            if (visited_ast->is_true)
                printf("True\n");
            else
                printf("False\n");
            break;
        }
        default:
            printf("%p\n", visited_ast);
            break;
        }
    }
    return init_ast(AST_NOOP);
}

visitor_T *init_visitor()
{
    visitor_T *visitor = calloc(1, sizeof(struct VISITOR_STRUCT));
    return visitor;
}

AST_T *visitor_visit(visitor_T *visitor, AST_T *node)
{
    // For debugging
    // printf("Visiting: %s\n", enum_names[node->type]);
    switch (node->type)
    {
    case AST_FUNC_CALL:
        return visitor_visit_func_call(visitor, node);
    case AST_FUNC_DEF:
        return visitor_visit_func_def(visitor, node);
    case AST_VAR_DEF:
        return visitor_visit_var_def(visitor, node);
    case AST_VAR:
        return visitor_visit_var(visitor, node);
    case AST_STR:
        return visitor_visit_str(visitor, node);
    case AST_INT:
        return visitor_visit_int(visitor, node);
    case AST_FLOAT:
        return visitor_visit_float(visitor, node);
    case AST_COMPOUND:
        return visitor_visit_compound(visitor, node);
    case AST_ADD:
        return visitor_visit_add(visitor, node);
    case AST_SUB:
        return visitor_visit_sub(visitor, node);
    case AST_AND:
        return visitor_visit_and(visitor, node);
    case AST_OR:
        return visitor_visit_or(visitor, node);
    case AST_MUL:
        return visitor_visit_mul(visitor, node);
    case AST_DIV:
        return visitor_visit_div(visitor, node);
    case AST_INT_DIV:
        return visitor_visit_int_div(visitor, node);
    case AST_IF_STMNT:
        return visitor_visit_if_statement(visitor, node);
    case AST_BOOL:
        return visitor_visit_bool(visitor, node);
    case AST_EQ_COMP:
        return visitor_visit_eq_comp(visitor, node);
    case AST_NEQ_COMP:
        return visitor_visit_not_eq_comp(visitor, node);
    case AST_LT_COMP:
        return visitor_visit_lt_comp(visitor, node);
    case AST_GT_COMP:
        return visitor_visit_gt_comp(visitor, node);
    case AST_LTE_COMP:
        return visitor_visit_lte_comp(visitor, node);
    case AST_GTE_COMP:
        return visitor_visit_gte_comp(visitor, node);
    case AST_RET_STMNT:
        return visitor_visit_ret_statement(visitor, node);
    case AST_NOOP:
        return node;
    default:
        printf("Uncaught statement of type: '%s'\n", enum_names[node->type]);
        exit(1);
    }
    return init_ast(AST_NOOP);
}

AST_T *visitor_visit_func_call(visitor_T *visitor, AST_T *node)
{
    if (strncmp(node->func_call_name, "print", 6) == 0)
        return builtin_function_print(visitor, node, node->func_call_args, node->func_call_args_size);

    AST_T *func_def = scope_get_func_def(
        node->global_scope,
        node->func_call_name);
    if (func_def->type == AST_NOOP)
    {
        printf("'%s' is not defined.\n", node->func_call_name);
        exit(1);
    }
    if (func_def->func_def_args_size != node->func_call_args_size)
    {
        printf("'%s' expected %ld args but recieved %ld.\n",
               node->func_call_name,
               func_def->func_def_args_size,
               node->func_call_args_size);
        exit(1);
    }

    AST_T *args[node->func_call_args_size];
    for (int i = 0; i < (int)node->func_call_args_size; i++)
    {
        AST_T *ast_var = (AST_T *)func_def->func_def_args[i];

        AST_T *ast_value = visitor_visit(visitor, (AST_T *)node->func_call_args[i]);
        AST_T *var_def = init_ast(AST_VAR_DEF);

        var_def->var_def_val = ast_value;
        var_def->var_def_var_name = calloc(strlen(ast_var->var_name) + 1, sizeof(char));
        strcpy(var_def->var_def_var_name, ast_var->var_name);
        var_def->scope = func_def->func_body->scope;
        args[i] = var_def;
    }
    // Add the param vars to function body scope after using current scopes' values
    for (int i = 0; i < (int)node->func_call_args_size; i++)
        scope_add_var_def(func_def->func_body->scope, args[i]);

    // Getting the number of variables declared in the function body
    int num_var_def_in_func = 0;
    for (int i = 0; i < func_def->func_body->compound_size; i++)
    {
        if (func_def->func_body->compound_val[i]->type == AST_VAR_DEF)
            num_var_def_in_func++;
    }
    // Keep track of the variables declared in the function body
    char *vars_to_be_removed[num_var_def_in_func];
    for (int i = 0; i < func_def->func_body->compound_size; i++)
    {
        if (func_def->func_body->compound_val[i]->type == AST_VAR_DEF)
            vars_to_be_removed[i] = func_def->func_body->compound_val[i]->var_def_var_name;
    }

    // Visit the function body
    AST_T *visited_body = visitor_visit(visitor, func_def->func_body);

    // Remove param variable declarations from the function body
    for (int i = 0; i < (int)node->func_call_args_size; i++)
        scope_rem_var_def(func_def->func_body->scope, func_def->func_def_args[i]->var_name);

    // Remove variables declared in function body
    for (int i = 0; i < num_var_def_in_func; i++)
        scope_rem_var_def(func_def->func_body->scope, vars_to_be_removed[i]);

    return visited_body;
}

AST_T *visitor_visit_func_def(visitor_T *visitor, AST_T *node)
{
    scope_add_func_def(node->global_scope, node);
    return node;
}

AST_T *visitor_visit_var_def(visitor_T *visitor, AST_T *node)
{
    node->var_def_val = visitor_visit(visitor, node->var_def_expr);
    scope_add_var_def(node->scope, node);
    return node;
}

AST_T *visitor_visit_var(visitor_T *visitor, AST_T *node)
{

    AST_T *vardef = scope_get_var_def(node->scope, node->var_name);
    AST_T *temp = node;
    while (vardef->type == AST_NOOP && temp->parent != (void *)0)
    {
        vardef = scope_get_var_def(temp->scope, node->var_name);
        temp = temp->parent;
    }
    if (vardef->type == AST_NOOP)
    {
        printf("Undefined variable '%s'\n", node->var_name);
        exit(1);
    }
    return visitor_visit(visitor, vardef->var_def_val);
}

AST_T *visitor_visit_str(visitor_T *visitor, AST_T *node)
{
    if (node->str_val != (void *)0)
        node->is_true = 1;
    return node;
}

AST_T *visitor_visit_and(visitor_T *visitor, AST_T *node)
{
    AST_T *ret = init_ast(AST_BOOL);
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    ret->is_true = left->is_true && right->is_true;
    return ret;
}

AST_T *visitor_visit_or(visitor_T *visitor, AST_T *node)
{
    AST_T *ret = init_ast(AST_BOOL);
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    ret->is_true = left->is_true || right->is_true;
    return ret;
}

AST_T *visitor_visit_int(visitor_T *visitor, AST_T *node)
{
    if (node->int_val != 0)
        node->is_true = 1;

    return node;
}

AST_T *visitor_visit_bool(visitor_T *visitor, AST_T *node)
{
    return node;
}

AST_T *visitor_visit_float(visitor_T *visitor, AST_T *node)
{
    if (node->float_val != 0)
        node->is_true = 1;
    return node;
}

AST_T *visitor_visit_add(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR);
            char *str = calloc(strlen(left->str_val) + strlen(right->str_val), sizeof(char));
            strcpy(str, left->str_val);
            ret->str_val = strcat(str, right->str_val);
            return ret;
        }
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = left->int_val + right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = left->float_val + right->float_val;
            return ret;
        }

        default:
        {
            printf("Cannot add objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT)
    {
        switch (right->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = left->float_val + right->int_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR);
            char str[171];
            sprintf(str, "%.2f%s", left->float_val, right->str_val);
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        default:
            break;
        }
    }
    else if (left->type == AST_INT)
    {

        switch (right->type)
        {
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = left->int_val + right->float_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR);
            char str[171];
            sprintf(str, "%d%s", left->int_val, right->str_val);
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        default:
            break;
        }
    }
    else if (left->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR);
        char str[171];
        switch (right->type)
        {
        case AST_FLOAT:
        {
            sprintf(str, "%s%.2f", left->str_val, right->float_val);
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_INT:
        {
            sprintf(str, "%s%d", left->str_val, right->int_val);
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_BOOL:
        {
            sprintf(str, "%s%s", left->str_val, right->is_true ? "True" : "False");
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_NOOP:
        {
            sprintf(str, "%sNULL", left->str_val);
            int len = strlen(str);
            ret->str_val = calloc(len, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        default:
            break;
        }
    }
    else if (left->type == AST_NOOP)
    {
        switch (right->type)
        {
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = right->float_val;
            return ret;
        }

        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = right->int_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR);
            ret->str_val = calloc(strlen(right->str_val) + 4, sizeof(char));
            strcat(ret->str_val, "NULL");
            strcat(ret->str_val, right->str_val);
            return ret;
        }
        default:
        {
            printf("bad operand type for unary +: '%s'\n", enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_BOOL && right->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR);
        char str[171];
        sprintf(str, "%s%s", left->is_true ? "True" : "False", right->str_val);
        int len = strlen(str);
        ret->str_val = calloc(len, sizeof(char));
        strncpy(ret->str_val, str, len);
        return ret;
    }
    printf("Cannot add objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
    exit(1);
}

AST_T *visitor_visit_sub(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = left->int_val - right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = left->float_val - right->float_val;
            return ret;
        }

        default:
            printf("Cannot subtract objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        ret->float_val = left->float_val - right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        ret->float_val = left->int_val - right->float_val;
        return ret;
    }
    else if (left->type == AST_NOOP)
    {
        switch (right->type)
        {
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = -1 * right->float_val;
            return ret;
        }

        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = -1 * right->int_val;
            return ret;
        }
        default:
        {
            printf("bad operand type for unary -: '%s'\n", enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (right->type == AST_NOOP)
    {
        printf("bad operand type for unary -: '%s'\n", enum_names[right->type]);
        exit(1);
    }
    else
    {
        printf("Cannot subtract objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_mul(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = left->int_val * right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            ret->float_val = left->float_val * right->float_val;
            return ret;
        }
        default:
        {
            printf("Cannot multiply objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        ret->float_val = left->float_val * (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        ret->float_val = (float)left->int_val * right->float_val;
        return ret;
    }
    else
    {
        printf("Cannot multiply objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_div(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            if (right->int_val == 0)
            {
                printf("Zero Division Error: Cannot Divide by 0.\n");
                exit(1);
            }
            ret->float_val = (float)left->int_val / (float)right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT);
            if (right->float_val == 0)
            {
                printf("Zero Division Error: Cannot Divide by 0.\n");
                exit(1);
            }
            ret->float_val = left->float_val / right->float_val;
            return ret;
        }

        default:
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        if (right->int_val == 0)
        {
            printf("Zero Division Error: Cannot Divide by 0.\n");
            exit(1);
        }
        ret->float_val = left->float_val / (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT);
        if (right->float_val == 0)
        {
            printf("Zero Division Error: Cannot Divide by 0.\n");
            exit(1);
        }
        ret->float_val = (float)left->int_val / right->float_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_not_eq_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val != right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val != right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true != right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strncmp(left->str_val, right->str_val, strnlen(left->str_val, 151)) != 0;
            return ret;
        }
        default:
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val != (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = (float)left->int_val != right->float_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_eq_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val == right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val == right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true == right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strcmp(left->str_val, right->str_val) == 0;
            return ret;
        }
        default:
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val == (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = (float)left->int_val == right->float_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_lt_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val < right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val < right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true < right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strcmp(left->str_val, right->str_val) < 0;
            return ret;
        }
        default:
        {
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type != AST_STR && right->type != AST_STR)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val < right->int_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_gt_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val > right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val > right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true > right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strcmp(left->str_val, right->str_val) > 0;
            return ret;
        }
        default:
        {
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type != AST_STR && right->type != AST_STR)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val > right->int_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_lte_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val <= right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val <= right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true <= right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strcmp(left->str_val, right->str_val) <= 0;
            return ret;
        }
        default:
        {
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type != AST_STR && right->type != AST_STR)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val < right->int_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_gte_comp(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->int_val >= right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->float_val >= right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = left->is_true >= right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL);
            ret->is_true = strcmp(left->str_val, right->str_val) >= 0;
            return ret;
        }
        default:
        {
            printf("Cannot compare objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type != AST_STR && right->type != AST_STR)
    {
        AST_T *ret = init_ast(AST_BOOL);
        ret->is_true = left->float_val >= right->int_val;
        return ret;
    }
    else
    {
        printf("Cannot compare objects of type '%s' and '%s'\n", enum_names[left->type], enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_int_div(visitor_T *visitor, AST_T *node)
{
    AST_T *left = visitor_visit(visitor, node->op_left);
    AST_T *right = visitor_visit(visitor, node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = left->int_val / right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_INT);
            ret->int_val = (int)left->float_val / (int)right->float_val;
            return ret;
        }

        default:
            printf("Cannot divide objects of type '%s'\n", enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_INT);
        ret->int_val = (int)left->float_val / right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_INT);
        ret->int_val = left->int_val / (int)right->float_val;
        return ret;
    }
    else
    {
        printf("Cannot divide objects of type '%s' and '%s'\n",
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *visitor_visit_if_statement(visitor_T *visitor, AST_T *node)
{
    AST_T *pred = visitor_visit(visitor, node->if_predicate);
    if (pred->is_true > 0)
        return visitor_visit(visitor, node->if_body);
    else if (node->else_body)
        return visitor_visit(visitor, node->else_body);
    return init_ast(AST_NOOP);
}

AST_T *visitor_visit_ret_statement(visitor_T *visitor, AST_T *node)
{
    return visitor_visit(visitor, node->return_expr);
}

AST_T *visitor_visit_compound(visitor_T *visitor, AST_T *node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        if (node->compound_val[i]->type == AST_RET_STMNT)
            return __visit_ret_stmnt__(visitor, node, node->compound_val[i]);

        AST_T *visit = visitor_visit(visitor, node->compound_val[i]);
        if (visit->type == AST_RET_STMNT)
            return __visit_ret_stmnt__(visitor, node, visit);
    }
    return init_ast(AST_NOOP);
}

AST_T *__visit_ret_stmnt__(visitor_T *visitor, AST_T *node, AST_T *to_visit)
{
    if (node->parent != NULL && node->parent->type != AST_FUNC_DEF)
    {
        AST_T *ret = init_ast(AST_RET_STMNT);
        ret->return_expr = visitor_visit(visitor, to_visit);
        ret->scope = node->scope;
        ret->global_scope = node->global_scope;
        ret->parent = node;
        return ret;
    }
    else
        return visitor_visit(visitor, to_visit);
}