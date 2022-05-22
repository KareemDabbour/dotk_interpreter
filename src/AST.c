#include "include/AST.h"
#include "include/scope.h"

#include <stdio.h>
#include <string.h>

static AST_T *builtin_function_print(AST_T *node, AST_T **args, size_t args_size);
static AST_T *builtin_function_len(AST_T *node, AST_T **args, size_t args_size);
static AST_T *__visit_ret_stmnt__(AST_T *node, AST_T *to_visit);
static AST_T *ast_visit_func_call(AST_T *node);
static AST_T *ast_visit_func_def(AST_T *node);
static AST_T *ast_visit_var_def(AST_T *node);
static AST_T *ast_visit_var_redef(AST_T *node);
static AST_T *ast_visit_var(AST_T *node);
static AST_T *ast_visit_arr(AST_T *node);
static AST_T *ast_visit_arr_def(AST_T *node);
static AST_T *ast_visit_arr_index(AST_T *node);
static AST_T *ast_visit_arr_index_assignment(AST_T *node);
static AST_T *ast_visit_str(AST_T *node);
static AST_T *ast_visit_compound(AST_T *node);
static AST_T *ast_visit_int(AST_T *node);
static AST_T *ast_visit_bool(AST_T *node);
static AST_T *ast_visit_float(AST_T *node);
static AST_T *ast_visit_add(AST_T *node);
static AST_T *ast_visit_sub(AST_T *node);
static AST_T *ast_visit_mul(AST_T *node);
static AST_T *ast_visit_mod(AST_T *node);
static AST_T *ast_visit_div(AST_T *node);
static AST_T *ast_visit_int_div(AST_T *node);
static AST_T *ast_visit_if_statement(AST_T *node);
static AST_T *ast_visit_ret_statement(AST_T *node);
static AST_T *ast_visit_and(AST_T *node);
static AST_T *ast_visit_or(AST_T *node);
static AST_T *ast_visit_not_eq_comp(AST_T *node);
static AST_T *ast_visit_eq_comp(AST_T *node);
static AST_T *ast_visit_lt_comp(AST_T *node);
static AST_T *ast_visit_gt_comp(AST_T *node);
static AST_T *ast_visit_lte_comp(AST_T *node);
static AST_T *ast_visit_gte_comp(AST_T *node);

const char *enum_names[31] = {
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
    "ARRAY",
    "ARRAY DECLARATION",
    "VARIABLE REDEFINITION",
    "ARRAY INDEXING ASSIGNMENT",
    "ARRAY INDEXING",
    "MODULUS"};

void print_arr(AST_T *arr)
{
    printf("[");
    if (arr->arr_size > 0)
    {
        for (int i = 0; i < arr->arr_size - 1; i++)
        {
            AST_T *temp = ast_visit(arr->arr[i]);

            switch (temp->type)
            {
            case AST_STR:
                printf("\"%s\", ", temp->str_val);
                break;
            case AST_INT:
                printf("%d, ", temp->int_val);
                break;
            case AST_FLOAT:
                printf("%.2f, ", temp->float_val);
                break;
            case AST_NOOP:
                printf("NULL, ");
                break;
            case AST_ARR:
            {
                if (temp == arr)
                    printf("[ ... ]");
                else
                    print_arr(temp);
                printf(", ");
                break;
            }
            case AST_BOOL:
            {
                if (temp->is_true)
                    printf("True, ");
                else
                    printf("False, ");
                break;
            }
            default:
                printf("%s, ", enum_names[temp->type]);
                break;
            }
        }

        AST_T *temp = ast_visit(arr->arr[arr->arr_size - 1]);

        switch (temp->type)
        {
        case AST_STR:
            printf("\"%s\"", temp->str_val);
            break;
        case AST_INT:
            printf("%d", temp->int_val);
            break;
        case AST_FLOAT:
            printf("%.2f", temp->float_val);
            break;
        case AST_NOOP:
            printf("NULL");
            break;
        case AST_ARR:
            if (temp == arr)
                printf("[ ... ]");
            else
                print_arr(temp);
            break;
        case AST_BOOL:
        {
            if (temp->is_true)
                printf("True");
            else
                printf("False");
            break;
        }
        default:
            printf("%s", enum_names[temp->type]);
            break;
        }
    }
    printf("]");
}

char *get_arr_as_string(AST_T *node)
{
    int str_size = 2;
    char *str = calloc(2, sizeof(char));
    str[0] = '[';
    str[1] = '\0';
    if (node->arr_size > 0)
    {
        for (int i = 0; i < node->arr_size - 1; i++)
        {
            AST_T *temp = node->arr[i];
            char str_temp[MAX_NUM_SPACE + MAX_STR_CHAR];
            int len;
            switch (temp->type)
            {
            case AST_STR:
            {
                sprintf(str_temp, "\"%s\", ", temp->str_val);
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
                break;
            }
            case AST_INT:
                sprintf(str_temp, "%d, ", temp->int_val);
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
                break;
            case AST_FLOAT:
                sprintf(str_temp, "%.2f, ", temp->float_val);
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
                break;
            case AST_NOOP:
                str = realloc(str, (7 + str_size) * sizeof(char));
                str_size += 7;
                strcat(str, "NULL, ");
                break;
            case AST_ARR:
            {
                if (temp == node)
                {
                    str = realloc(str, (10 + str_size) * sizeof(char));
                    str_size += 10;
                    strcat(str, "[ ... ], ");
                }
                else
                {
                    char *arr_str = get_arr_as_string(temp);
                    sprintf(str_temp, "%s, ", arr_str); // get_arr_as_string(temp));
                    free(arr_str);
                    len = strlen(str_temp);
                    str = realloc(str, (len + str_size) * sizeof(char));
                    str_size += len;
                    strcat(str, str_temp);
                }
                break;
            }
            case AST_BOOL:
            {
                sprintf(str_temp, "%s, ", temp->is_true ? "True" : "False");
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
                break;
            }
            default:
                sprintf(str_temp, "%s, ", enum_names[temp->type]);
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
                break;
            }
        }

        AST_T *temp = node->arr[node->arr_size - 1];
        char str_temp[MAX_NUM_SPACE + MAX_STR_CHAR];
        int len;
        switch (temp->type)
        {
        case AST_STR:

            sprintf(str_temp, "\"%s\"", temp->str_val);
            len = strlen(str_temp);
            str = realloc(str, (len + str_size) * sizeof(char));
            str_size += len;
            strcat(str, str_temp);
            break;
        case AST_INT:
            sprintf(str_temp, "%d", temp->int_val);
            len = strlen(str_temp);
            str = realloc(str, (len + str_size) * sizeof(char));
            str_size += len;
            strcat(str, str_temp);
            break;
        case AST_FLOAT:
            sprintf(str_temp, "%.2f", temp->float_val);
            len = strlen(str_temp);
            str = realloc(str, (len + str_size) * sizeof(char));
            str_size += len;
            strcat(str, str_temp);
            break;
        case AST_NOOP:
            str = realloc(str, (5 + str_size) * sizeof(char));
            str_size += 5;
            strcat(str, "NULL");
            break;
        case AST_ARR:
        {
            if (temp == node)
            {
                str = realloc(str, (8 + str_size) * sizeof(char));
                str_size += 8;
                strcat(str, "[ ... ]");
            }
            else
            {
                char *arr_str = get_arr_as_string(temp);
                sprintf(str_temp, "%s", arr_str); // get_arr_as_string(temp));
                free(arr_str);
                len = strlen(str_temp);
                str = realloc(str, (len + str_size) * sizeof(char));
                str_size += len;
                strcat(str, str_temp);
            }
            break;
        }
        case AST_BOOL:
        {
            sprintf(str_temp, "%s", temp->is_true ? "True" : "False");
            len = strlen(str_temp);
            str = realloc(str, (len + str_size) * sizeof(char));
            str_size += len;
            strcat(str, str_temp);
            break;
        }
        default:
            sprintf(str_temp, "%s", enum_names[temp->type]);
            len = strlen(str_temp);
            str = realloc(str, (len + str_size) * sizeof(char));
            str_size += len;
            strcat(str, str_temp);
            break;
        }
    }
    str_size = strlen(str) + 1;
    str = realloc(str, ++str_size * sizeof(char));
    str[str_size - 2] = ']';
    str[str_size - 1] = '\0';
    return str;
}

int __compare_arr__(AST_T *x, AST_T *y, int operator)
{
    int ret = 1;

    if (x->arr_size != y->arr_size)
        ret = 0;
    else
    {
        for (int i = 0; i < x->arr_size; i++)
        {
            AST_T *x_elem = ast_visit(x->arr[i]);
            AST_T *y_elem = ast_visit(y->arr[i]);
            if (x_elem->type == y_elem->type)
            {
                switch (x_elem->type)
                {
                case AST_INT:
                {
                    switch (operator)
                    {
                    case AST_EQ_COMP:
                        ret = x_elem->int_val == y_elem->int_val;
                        break;
                    case AST_NEQ_COMP:
                        ret = x_elem->int_val != y_elem->int_val;
                        break;
                    case AST_LT_COMP:
                        ret = x_elem->int_val < y_elem->int_val;
                        break;
                    case AST_LTE_COMP:
                        ret = x_elem->int_val <= y_elem->int_val;
                        break;
                    case AST_GT_COMP:
                        ret = x_elem->int_val > y_elem->int_val;
                        break;
                    case AST_GTE_COMP:
                        ret = x_elem->int_val >= y_elem->int_val;
                        break;
                    default:
                        ret = 0;
                        break;
                    }
                    break;
                }
                case AST_FLOAT:
                {
                    switch (operator)
                    {
                    case AST_EQ_COMP:
                        ret = x_elem->float_val == y_elem->float_val;
                        break;
                    case AST_NEQ_COMP:
                        ret = x_elem->float_val != y_elem->float_val;
                        break;
                    case AST_LT_COMP:
                        ret = x_elem->float_val < y_elem->float_val;
                        break;
                    case AST_LTE_COMP:
                        ret = x_elem->float_val <= y_elem->float_val;
                        break;
                    case AST_GT_COMP:
                        ret = x_elem->float_val > y_elem->float_val;
                        break;
                    case AST_GTE_COMP:
                        ret = x_elem->float_val >= y_elem->float_val;
                        break;
                    default:
                        ret = 0;
                        break;
                    }
                    break;
                }
                case AST_BOOL:
                {
                    switch (operator)
                    {
                    case AST_EQ_COMP:
                        ret = x_elem->is_true == y_elem->is_true;
                        break;
                    case AST_NEQ_COMP:
                        ret = x_elem->is_true != y_elem->is_true;
                        break;
                    case AST_LT_COMP:
                        ret = x_elem->is_true < y_elem->is_true;
                        break;
                    case AST_LTE_COMP:
                        ret = x_elem->is_true <= y_elem->is_true;
                        break;
                    case AST_GT_COMP:
                        ret = x_elem->is_true > y_elem->is_true;
                        break;
                    case AST_GTE_COMP:
                        ret = x_elem->is_true >= y_elem->is_true;
                        break;
                    default:
                        ret = 0;
                        break;
                    }
                    break;
                }
                case AST_STR:
                {
                    switch (operator)
                    {
                    case AST_EQ_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) == 0;
                        break;
                    case AST_NEQ_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) != 0;
                        break;
                    case AST_LT_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) < 0;
                        break;
                    case AST_LTE_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) <= 0;
                        break;
                    case AST_GT_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) > 0;
                        break;
                    case AST_GTE_COMP:
                        ret = strcmp(x_elem->str_val, y_elem->str_val) >= 0;
                        break;
                    default:
                        ret = 0;
                        break;
                    }
                    break;
                }
                case AST_ARR:
                {
                    if (x_elem->arr_size != y_elem->arr_size)
                    {
                        ret = 0;
                        break;
                    }
                    else
                    {
                        for (int j = 0; j < x_elem->arr_size; j++)
                        {
                            AST_T *x_arr_elem = x_elem->arr[j];
                            AST_T *y_arr_elem = y_elem->arr[j];
                            if (x_arr_elem->type == y_arr_elem->type)
                            {
                                switch (x_arr_elem->type)
                                {
                                case AST_INT:
                                {
                                    switch (operator)
                                    {
                                    case AST_EQ_COMP:
                                        ret = x_arr_elem->int_val == y_arr_elem->int_val;
                                        break;
                                    case AST_NEQ_COMP:
                                        ret = x_arr_elem->int_val != y_arr_elem->int_val;
                                        break;
                                    case AST_LT_COMP:
                                        ret = x_arr_elem->int_val < y_arr_elem->int_val;
                                        break;
                                    case AST_LTE_COMP:
                                        ret = x_arr_elem->int_val <= y_arr_elem->int_val;
                                        break;
                                    case AST_GT_COMP:
                                        ret = x_arr_elem->int_val > y_arr_elem->int_val;
                                        break;
                                    case AST_GTE_COMP:
                                        ret = x_arr_elem->int_val >= y_arr_elem->int_val;
                                        break;
                                    default:
                                        ret = 0;
                                        break;
                                    }
                                    break;
                                }
                                case AST_FLOAT:
                                {
                                    switch (operator)
                                    {
                                    case AST_EQ_COMP:
                                        ret = x_arr_elem->float_val == y_arr_elem->float_val;
                                        break;
                                    case AST_NEQ_COMP:
                                        ret = x_arr_elem->float_val != y_arr_elem->float_val;
                                        break;
                                    case AST_LT_COMP:
                                        ret = x_arr_elem->float_val < y_arr_elem->float_val;
                                        break;
                                    case AST_LTE_COMP:
                                        ret = x_arr_elem->float_val <= y_arr_elem->float_val;
                                        break;
                                    case AST_GT_COMP:
                                        ret = x_arr_elem->float_val > y_arr_elem->float_val;
                                        break;
                                    case AST_GTE_COMP:
                                        ret = x_arr_elem->float_val >= y_arr_elem->float_val;
                                        break;
                                    default:
                                        ret = 0;
                                        break;
                                    }
                                    break;
                                }
                                case AST_BOOL:
                                {
                                    switch (operator)
                                    {
                                    case AST_EQ_COMP:
                                        ret = x_arr_elem->is_true == y_arr_elem->is_true;
                                        break;
                                    case AST_NEQ_COMP:
                                        ret = x_arr_elem->is_true != y_arr_elem->is_true;
                                        break;
                                    case AST_LT_COMP:
                                        ret = x_arr_elem->is_true < y_arr_elem->is_true;
                                        break;
                                    case AST_LTE_COMP:
                                        ret = x_arr_elem->is_true <= y_arr_elem->is_true;
                                        break;
                                    case AST_GT_COMP:
                                        ret = x_arr_elem->is_true > y_arr_elem->is_true;
                                        break;
                                    case AST_GTE_COMP:
                                        ret = x_arr_elem->is_true >= y_arr_elem->is_true;
                                        break;
                                    default:
                                        ret = 0;
                                        break;
                                    }
                                    break;
                                }
                                case AST_STR:
                                {
                                    switch (operator)
                                    {
                                    case AST_EQ_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) == 0;
                                        break;
                                    case AST_NEQ_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) != 0;
                                        break;
                                    case AST_LT_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) < 0;
                                        break;
                                    case AST_LTE_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) <= 0;
                                        break;
                                    case AST_GT_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) > 0;
                                        break;
                                    case AST_GTE_COMP:
                                        ret = strcmp(x_arr_elem->str_val, y_arr_elem->str_val) >= 0;
                                        break;
                                    default:
                                        ret = 0;
                                        break;
                                    }
                                    break;
                                }
                                case AST_ARR:
                                {
                                    if (x_arr_elem->arr_size != y_arr_elem->arr_size)
                                    {
                                        ret = 0;
                                        break;
                                    }
                                    else
                                    {
                                        ret = __compare_arr__(x_arr_elem, y_arr_elem, operator);
                                        break;
                                    }
                                    break;
                                }
                                default:
                                    printf("%d:%d -- Cannot compare objects of type '%s'\n",
                                           x->line,
                                           x->col,
                                           enum_names[x->type]);
                                    exit(1);
                                }
                            }
                            else if (x_arr_elem->type == AST_FLOAT && y_arr_elem->type == AST_INT)
                            {
                                ret = x_arr_elem->float_val < (float)y_arr_elem->int_val;
                                break;
                            }
                            else if (x_arr_elem->type == AST_INT && y_arr_elem->type == AST_FLOAT)
                            {
                                ret = (float)x_arr_elem->int_val < y_arr_elem->float_val;
                                break;
                            }
                            else
                            {
                                printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
                                       x_arr_elem->line,
                                       x_arr_elem->col,
                                       enum_names[x_arr_elem->type],
                                       enum_names[y_arr_elem->type]);
                                exit(1);
                            }
                            if (ret == 0)
                                break;
                        }
                    }
                    break;
                }
                default:
                    printf("%d:%d -- Cannot compare objects of type '%s'\n",
                           x_elem->line,
                           x_elem->col,
                           enum_names[x_elem->type]);
                    exit(1);
                }
            }
            else if (x_elem->type == AST_FLOAT && y_elem->type == AST_INT)
            {
                ret = x_elem->float_val < (float)y_elem->int_val;
                break;
            }
            else if (x_elem->type == AST_INT && y_elem->type == AST_FLOAT)
            {
                ret = (float)x_elem->int_val < y_elem->float_val;
                break;
            }
            else
            {
                printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
                       x_elem->line,
                       x_elem->col,
                       enum_names[x_elem->type],
                       enum_names[y_elem->type]);
                exit(1);
            }
            if (ret == 0)
                break;
        }
    }
    return ret;
}

AST_T *builtin_function_len(AST_T *node, AST_T **args, size_t args_size)
{
    if (args_size == 0 || args_size > 1)
    {
        printf("%d:%d -- 'len()' takes exactly one argument (%ld given)\n",
               node->line,
               node->col,
               args_size);
        exit(1);
    }
    AST_T *ret = init_ast(AST_INT, node->line, node->col);
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    AST_T *visited_ast = ast_visit(args[0]);
    switch (visited_ast->type)
    {
    case AST_STR:
        ret->int_val = strlen(visited_ast->str_val);
        break;
    case AST_ARR:
        ret->int_val = visited_ast->arr_size;
        break;

    default:
        printf("%d:%d -- Cannot get length of type '%s'.", node->line, node->col, enum_names[visited_ast->type]);
        exit(1);
    }
    return ret;
}

AST_T *builtin_function_print(AST_T *node, AST_T **args, size_t args_size)
{
    for (size_t i = 0; i < args_size; i++)
    {
        AST_T *visited_ast = ast_visit(args[i]);

        switch (visited_ast->type)
        {
        case AST_STR:
            printf("\"%s\"\n", visited_ast->str_val);
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
        case AST_ARR:
            print_arr(visited_ast);
            printf("\n");
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
            printf("%s\n", enum_names[visited_ast->type]);
            break;
        }
    }
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *ast_visit(AST_T *node)
{
    // For debugging
    // printf("Visiting: %s\n", enum_names[node->type]);
    switch (node->type)
    {
    case AST_FUNC_CALL:
        return ast_visit_func_call(node);
    case AST_FUNC_DEF:
        return ast_visit_func_def(node);
    case AST_VAR_DEF:
        return ast_visit_var_def(node);
    case AST_VAR_REDEF:
        return ast_visit_var_redef(node);
    case AST_VAR:
        return ast_visit_var(node);
    case AST_STR:
        return ast_visit_str(node);
    case AST_INT:
        return ast_visit_int(node);
    case AST_FLOAT:
        return ast_visit_float(node);
    case AST_COMPOUND:
        return ast_visit_compound(node);
    case AST_ADD:
        return ast_visit_add(node);
    case AST_SUB:
        return ast_visit_sub(node);
    case AST_AND:
        return ast_visit_and(node);
    case AST_OR:
        return ast_visit_or(node);
    case AST_MUL:
        return ast_visit_mul(node);
    case AST_MOD:
        return ast_visit_mod(node);
    case AST_DIV:
        return ast_visit_div(node);
    case AST_INT_DIV:
        return ast_visit_int_div(node);
    case AST_IF_STMNT:
        return ast_visit_if_statement(node);
    case AST_BOOL:
        return ast_visit_bool(node);
    case AST_EQ_COMP:
        return ast_visit_eq_comp(node);
    case AST_NEQ_COMP:
        return ast_visit_not_eq_comp(node);
    case AST_LT_COMP:
        return ast_visit_lt_comp(node);
    case AST_GT_COMP:
        return ast_visit_gt_comp(node);
    case AST_LTE_COMP:
        return ast_visit_lte_comp(node);
    case AST_GTE_COMP:
        return ast_visit_gte_comp(node);
    case AST_RET_STMNT:
        return ast_visit_ret_statement(node);
    case AST_ARR:
        return ast_visit_arr(node);
    case AST_ARR_DEF:
        return ast_visit_arr_def(node);
    case AST_ARR_INDEX_ASSIGNMENT:
        return ast_visit_arr_index_assignment(node);
    case AST_ARR_INDEX:
        return ast_visit_arr_index(node);
    case AST_NOOP:
        return node;
    default:
        printf("%d:%d -- Uncaught statement of type: '%s'\n", node->line, node->col, enum_names[node->type]);
        exit(1);
    }
}

AST_T *ast_visit_func_call(AST_T *node)
{
    if (strncmp(node->func_call_name, "print", 6) == 0)
        return builtin_function_print(node, node->func_call_args, node->func_call_args_size);
    if (strncmp(node->func_call_name, "len", 4) == 0)
        return builtin_function_len(node, node->func_call_args, node->func_call_args_size);

    AST_T *func_def = scope_get_func_def(
        node->global_scope,
        node->func_call_name);
    if (func_def->type == AST_NOOP)
    {
        printf("%d:%d -- '%s' is not defined.\n",
               node->line,
               node->col,
               node->func_call_name);
        exit(1);
    }
    if (func_def->func_def_args_size != node->func_call_args_size)
    {
        printf("%d:%d -- '%s' expected %ld args but recieved %ld.\n",
               node->line,
               node->col,
               node->func_call_name,
               func_def->func_def_args_size,
               node->func_call_args_size);
        exit(1);
    }

    AST_T *args[node->func_call_args_size];
    for (int i = 0; i < (int)node->func_call_args_size; i++)
    {
        AST_T *ast_var = (AST_T *)func_def->func_def_args[i];

        AST_T *ast_value = ast_visit((AST_T *)node->func_call_args[i]);
        AST_T *var_def = init_ast(AST_VAR_DEF, ast_value->line, ast_value->col);

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
    AST_T *visited_body = ast_visit(func_def->func_body);

    // Remove param variable declarations from the function body
    for (int i = 0; i < (int)node->func_call_args_size; i++)
        scope_rem_var_def(func_def->func_body->scope, func_def->func_def_args[i]->var_name);

    // Remove variables declared in function body
    for (int i = 0; i < num_var_def_in_func; i++)
        scope_rem_var_def(func_def->func_body->scope, vars_to_be_removed[i]);

    return visited_body;
}

AST_T *ast_visit_func_def(AST_T *node)
{
    scope_add_func_def(node->global_scope, node);
    return node;
}

AST_T *ast_visit_var_def(AST_T *node)
{
    node->var_def_val = ast_visit(node->var_def_expr);
    scope_add_var_def(node->scope, node);
    return node;
}

AST_T *ast_visit_arr(AST_T *node)
{
    if (node->arr_size > 0)
        node->is_true = 1;
    return node;
}

AST_T *ast_visit_arr_def(AST_T *node)
{

    AST_T *ret = init_ast(AST_ARR, node->line, node->col);
    if (node->arr_size_expr == (void *)0)
    {
        ret->arr = calloc(node->arr_size, sizeof(struct AST_STRUCT *));
        ret->arr_size = node->arr_size;
        for (int i = 0; i < ret->arr_size; i++)
        {
            ret->arr[i] = ast_visit(node->compound_val[i]);
            ret->arr[i]->scope = node->scope;
            ret->arr[i]->global_scope = node->global_scope;
            ret->arr[i]->parent = node->parent;
        }

        ret->scope = node->scope;
        ret->parent = node->parent;
        return ret;
    }
    AST_T *size = ast_visit(node->arr_size_expr);
    if (size->type != AST_INT)
    {
        printf("%d:%d -- Can't create an array with size of type '%s' must be 'INT' \n",
               node->line,
               node->col,
               enum_names[size->type]);
        exit(1);
    }
    if (size->int_val < 0)
    {
        printf("%d:%d -- Can't create an array with size below 0\n",
               node->line,
               node->col);
        exit(1);
    }
    ret->arr_size = size->int_val;
    ret->arr = calloc(ret->arr_size, sizeof(AST_T *));
    if (node->arr_inner_index == NULL)
    {
        if (node->var_def_val == NULL)
        {
            for (int i = 0; i < ret->arr_size; i++)
                ret->arr[i] = init_ast(AST_NOOP, node->line, node->col);
        }
        else
        {
            for (int i = 0; i < ret->arr_size; i++)
                ret->arr[i] = ast_visit(node->var_def_val);
        }
    }
    else
    {
        node->arr_inner_index->var_def_val = node->var_def_val;
        node->arr_inner_index->scope = node->scope;
        for (int i = 0; i < ret->arr_size; i++)
            ret->arr[i] = ast_visit(node->arr_inner_index);
    }
    ret->scope = node->scope;
    ret->parent = node->parent;

    return ret;
}

AST_T *ast_visit_arr_index(AST_T *node)
{
    AST_T *arr;
    if (node->arr == NULL)
    {
        AST_T *aux = scope_get_var_def(node->scope, node->var_def_var_name);
        AST_T *temp = node;
        while (aux->type == AST_NOOP && temp->parent != (void *)0)
        {
            aux = scope_get_var_def(temp->scope, node->var_def_var_name);
            temp = temp->parent;
        }
        if (aux->type == AST_NOOP)
        {
            printf("%d:%d -- Undefined variable '%s'\n",
                   node->line,
                   node->col,
                   node->var_def_var_name);
            exit(1);
        }
        arr = aux->var_def_val;
    }
    else
    {
        arr = init_ast(AST_ARR, node->line, node->col);
        arr->arr = node->arr;
        arr->arr_size = node->arr_size;

        arr->scope = node->scope;
        arr->global_scope = node->global_scope;
    }

    if (arr->type != AST_ARR && arr->type != AST_STR)
    {
        printf("%d:%d -- '%s' type var '%s' is not subscriptable\n",
               node->line,
               node->col,
               enum_names[arr->type],
               node->var_def_var_name);
        exit(1);
    }

    AST_T *ast_index = ast_visit(node->arr_index);
    if (ast_index->type != AST_INT)
    {
        printf("%d:%d -- Can't index an array with type '%s' must be 'INT' \n",
               node->line,
               node->col,
               enum_names[ast_index->type]);
        exit(1);
    }
    size_t index;
    if (ast_index->int_val < 0)
        index = arr->arr_size + ast_index->int_val;
    else
        index = ast_index->int_val;
    if (index < 0 || index >= arr->arr_size)
    {
        printf("%d:%d -- Index Out of Bounds Error. Var '%s' has size %ld but %d was passed in\n",
               node->line,
               node->col,
               node->var_def_var_name,
               arr->arr_size,
               ast_index->int_val);
        exit(1);
    }
    AST_T *ret;
    if (arr->type == AST_ARR)
    {
        for (int i = 0; i < arr->arr_size; i++)
            arr->arr[i]->scope = node->scope;
        ret = ast_visit(arr->arr[index]);
        if (node->arr_inner_index != NULL)
        {
            if (ret->type != AST_ARR)
            {
                printf("%d:%d -- '%s' type is not subscriptable\n",
                       node->line,
                       node->col,
                       enum_names[ret->type]);
                exit(1);
            }
            node->arr_inner_index->arr = ret->arr;
            node->arr_inner_index->arr_size = ret->arr_size;
            return ast_visit(node->arr_inner_index);
        }
    }
    else if (arr->type == AST_STR)
    {
        ret = init_ast(AST_STR, node->line, node->col);
        ret->str_val = calloc(2, sizeof(char));
        sprintf(ret->str_val, "%c", arr->str_val[index]);
        ret->str_val[1] = '\0';
        ret->scope = node->scope;
        ret->parent = node->parent;
    }
    return ret;
}

AST_T *ast_visit_arr_index_assignment(AST_T *node)
{
    AST_T *arr;
    if (node->arr == NULL)
    {
        AST_T *aux = scope_get_var_def(node->scope, node->var_def_var_name);
        AST_T *temp = node;
        while (aux->type == AST_NOOP && temp->parent != (void *)0)
        {
            aux = scope_get_var_def(temp->scope, node->var_def_var_name);
            temp = temp->parent;
        }
        if (aux->type == AST_NOOP)
        {
            printf("%d:%d -- Undefined variable '%s'\n",
                   node->line,
                   node->col,
                   node->var_def_var_name);
            exit(1);
        }
        arr = aux->var_def_val;
    }
    else
    {
        arr = init_ast(AST_ARR, node->line, node->col);
        arr->arr = node->arr;
        arr->arr_size = node->arr_size;
    }

    if (arr->type != AST_ARR)
    {
        printf("%d:%d -- '%s' type var '%s' does not support item assignment \n",
               node->line,
               node->col,
               enum_names[arr->type],
               node->var_def_var_name);
        exit(1);
    }
    AST_T *ast_index = ast_visit(node->arr_index);
    if (ast_index->type != AST_INT)
    {
        printf("%d:%d -- Can't index an array with type '%s' must be 'INT' \n",
               node->line,
               node->col,
               enum_names[ast_index->type]);
        exit(1);
    }
    size_t index;
    if (ast_index->int_val < 0)
        index = arr->arr_size + ast_index->int_val;
    else
        index = ast_index->int_val;
    if (index < 0 || index >= arr->arr_size)
    {
        printf("%d:%d -- Index Out of Bounds Error '%s' has size %ld but %ld was passed in\n",
               node->line,
               node->col,
               node->var_def_var_name,
               arr->arr_size,
               index);
        exit(1);
    }

    if (node->arr_inner_index != NULL)
    {
        AST_T *inner_arr = ast_visit(arr->arr[index]);
        if (inner_arr->type != AST_ARR)
        {
            printf("%d:%d -- '%s' type is not subscriptable\n",
                   node->line,
                   node->col,
                   enum_names[inner_arr->type]);
            exit(1);
        }
        node->arr_inner_index->arr = inner_arr->arr;
        node->arr_inner_index->arr_size = inner_arr->arr_size;
        node->arr_inner_index->arr_new_val = node->arr_new_val;
        return ast_visit(node->arr_inner_index);
    }
    else
    {
        AST_T *ret = ast_visit(node->arr_new_val);
        arr->arr[index] = ret;
        return node;
    }
}

AST_T *ast_visit_var_redef(AST_T *node)
{
    AST_T *vardef = scope_get_var_def(node->scope, node->var_def_var_name);
    AST_T *temp = node;
    while (vardef->type == AST_NOOP && temp->parent != (void *)0)
    {
        vardef = scope_get_var_def(temp->scope, node->var_def_var_name);
        temp = temp->parent;
    }
    if (vardef->type == AST_NOOP)
    {
        printf("%d:%d -- Undefined variable '%s'\n",
               node->line,
               node->col,
               node->var_name);
        exit(1);
    }

    vardef->var_def_val = ast_visit(node->var_def_expr);
    scope_replace_var_def(vardef->scope, vardef);

    return vardef;
}

AST_T *ast_visit_var(AST_T *node)
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
        printf("%d:%d -- Undefined variable '%s'\n",
               node->line,
               node->col,
               node->var_name);
        exit(1);
    }
    return ast_visit(vardef->var_def_val);
}

AST_T *ast_visit_str(AST_T *node)
{
    if (node->str_val != (void *)0)
        node->is_true = 1;
    return node;
}

AST_T *ast_visit_and(AST_T *node)
{
    AST_T *ret = init_ast(AST_BOOL, node->line, node->col);
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    ret->is_true = left->is_true && right->is_true;
    return ret;
}

AST_T *ast_visit_or(AST_T *node)
{
    AST_T *ret = init_ast(AST_BOOL, node->line, node->col);
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    ret->is_true = left->is_true || right->is_true;
    return ret;
}

AST_T *ast_visit_int(AST_T *node)
{
    if (node->int_val != 0)
        node->is_true = 1;

    return node;
}

AST_T *ast_visit_bool(AST_T *node)
{
    return node;
}

AST_T *ast_visit_float(AST_T *node)
{
    if (node->float_val != 0)
        node->is_true = 1;
    return node;
}

AST_T *ast_visit_add(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            char *str = calloc(strlen(left->str_val) + strlen(right->str_val) + 1, sizeof(char));
            strcpy(str, left->str_val);
            ret->str_val = strcat(str, right->str_val);
            return ret;
        }
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = left->int_val + right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = left->float_val + right->float_val;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_ARR, left->line, left->col);
            ret->arr_size = left->arr_size + right->arr_size;
            ret->arr = calloc(ret->arr_size, sizeof(struct AST_STRUCT *));
            for (int i = 0; i < left->arr_size; i++)
                ret->arr[i] = left->arr[i];
            int k = left->arr_size;
            for (int i = 0; i < right->arr_size; i++)
                ret->arr[k++] = right->arr[i];
            return ret;
        }

        default:
        {
            printf("%d:%d -- Cannot add objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
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
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = left->float_val + right->int_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            char str[MAX_NUM_SPACE + MAX_STR_CHAR];
            sprintf(str, "%.2f%s", left->float_val, right->str_val);
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            printf("%d:%d -- Can only concatenate array (not 'FLOAT') to array\n",
                   left->line,
                   left->col);
            exit(1);
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
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = left->int_val + right->float_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            char str[MAX_NUM_SPACE + MAX_STR_CHAR];
            sprintf(str, "%d%s", left->int_val, right->str_val);
            int len = strlen(str) + 1;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            printf("%d:%d -- Can only concatenate array (not 'INT') to array\n",
                   left->line,
                   left->col);
            exit(1);
        }
        default:
            break;
        }
    }
    else if (left->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR, left->line, left->col);
        char str[MAX_NUM_SPACE + MAX_STR_CHAR];
        switch (right->type)
        {
        case AST_FLOAT:
        {
            sprintf(str, "%s%.2f", left->str_val, right->float_val);
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_INT:
        {
            sprintf(str, "%s%d", left->str_val, right->int_val);
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_BOOL:
        {
            sprintf(str, "%s%s", left->str_val, right->is_true ? "True" : "False");
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_NOOP:
        {
            sprintf(str, "%sNULL", left->str_val);
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            char *arr_str = get_arr_as_string(right);
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            char *str = calloc(strlen(left->str_val) + strlen(arr_str) + 1, sizeof(char));
            strcpy(str, left->str_val);
            ret->str_val = strcat(str, arr_str);
            free(arr_str);
            return ret;

            printf("%d:%d -- Can only concatenate array (not 'STR') to array\n",
                   left->line,
                   left->col);
            exit(1);
        }
        default:
            break;
        }
    }
    else if (left->type == AST_ARR)
    {
        if (right->type == AST_STR)
        {
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            char str[MAX_NUM_SPACE + MAX_STR_CHAR];
            char *arr_str = get_arr_as_string(left);
            sprintf(str, "%s%s", arr_str, right->str_val);
            free(arr_str);
            int len = strlen(str);
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        printf("%d:%d -- Can only concatenate array (not '%s') to array\n",
               left->line,
               left->col,
               enum_names[right->type]);
        exit(1);
    }
    else if (left->type == AST_NOOP)
    {
        switch (right->type)
        {
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = right->float_val;
            return ret;
        }
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = right->int_val;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            ret->str_val = calloc(strlen(right->str_val) + 5, sizeof(char));
            strcat(ret->str_val, "NULL");
            strcat(ret->str_val, right->str_val);
            return ret;
        }
        default:
        {
            printf("%d:%d -- bad operand type for unary operator '+': '%s'\n",
                   right->line,
                   right->col,
                   enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_BOOL && right->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR, left->line, left->col);
        char str[MAX_NUM_SPACE + MAX_STR_CHAR];
        sprintf(str, "%s%s", left->is_true ? "True" : "False", right->str_val);
        int len = strlen(str);
        ret->str_val = calloc(len + 1, sizeof(char));
        strncpy(ret->str_val, str, len);
        return ret;
    }
    printf("%d:%d -- Cannot add objects of type '%s' and '%s'\n",
           left->line,
           left->col,
           enum_names[left->type],
           enum_names[right->type]);
    exit(1);
}

AST_T *ast_visit_sub(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = left->int_val - right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = left->float_val - right->float_val;
            return ret;
        }

        default:
            printf("%d:%d -- Cannot subtract objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        ret->float_val = left->float_val - right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        ret->float_val = left->int_val - right->float_val;
        return ret;
    }
    else if (left->type == AST_NOOP)
    {
        switch (right->type)
        {
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = -1 * right->float_val;
            return ret;
        }

        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = -1 * right->int_val;
            return ret;
        }
        default:
        {

            printf("%d:%d -- bad operand type for unary operator '-': '%s'\n",
                   right->line,
                   right->col,
                   enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (right->type == AST_NOOP)
    {
        printf("%d:%d -- bad operand type for unary operator '-': '%s'\n",
               right->line,
               right->col,
               enum_names[right->type]);
        exit(1);
    }
    else
    {
        printf("%d:%d -- Cannot subtract objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_mod(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = left->int_val % right->int_val;
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot perfom modulo on objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else
    {
        printf("%d:%d -- Cannot perfom modulo on objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_mul(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = left->int_val * right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            ret->float_val = left->float_val * right->float_val;
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot multiply objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        ret->float_val = left->float_val * (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        ret->float_val = (float)left->int_val * right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot multiply objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_div(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            if (right->int_val == 0)
            {
                printf("%d:%d -- Zero Division Error: Cannot Divide by 0.\n", right->line, right->col);
                exit(1);
            }
            ret->float_val = (float)left->int_val / (float)right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
            if (right->float_val == 0)
            {
                printf("%d:%d -- Zero Division Error: Cannot Divide by 0.\n", right->line, right->col);
                exit(1);
            }
            ret->float_val = left->float_val / right->float_val;
            return ret;
        }

        default:
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        if (right->int_val == 0)
        {
            printf("%d:%d -- Zero Division Error: Cannot Divide by 0.\n", right->line, right->col);
            exit(1);
        }
        ret->float_val = left->float_val / (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        if (right->float_val == 0)
        {
            printf("%d:%d -- Zero Division Error: Cannot Divide by 0.\n", right->line, right->col);
            exit(1);
        }
        ret->float_val = (float)left->int_val / right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_not_eq_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val != right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val != right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true != right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strncmp(left->str_val, right->str_val, MAX_STR_CHAR) != 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_NEQ_COMP);
            return ret;
        }
        default:
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val != (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val != right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_eq_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val == right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val == right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true == right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strcmp(left->str_val, right->str_val) == 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_EQ_COMP);
            return ret;
        }
        default:
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val == (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val == right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_lt_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val < right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val < right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true < right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strcmp(left->str_val, right->str_val) < 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_LT_COMP);
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val < (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val < right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_gt_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val > right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val > right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true > right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strcmp(left->str_val, right->str_val) > 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_GT_COMP);
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val > (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val > right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_lte_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val <= right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val <= right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true <= right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strcmp(left->str_val, right->str_val) <= 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_LTE_COMP);
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val <= (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val <= right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col, enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_gte_comp(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->int_val >= right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->float_val >= right->float_val;
            return ret;
        }
        case AST_BOOL:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = left->is_true >= right->is_true;
            return ret;
        }
        case AST_STR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = strcmp(left->str_val, right->str_val) >= 0;
            return ret;
        }
        case AST_ARR:
        {
            AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
            ret->is_true = __compare_arr__(left, right, AST_GTE_COMP);
            return ret;
        }
        default:
        {
            printf("%d:%d -- Cannot compare objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = left->float_val >= (float)right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_BOOL, left->line, left->col);
        ret->is_true = (float)left->int_val >= right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               left->line,
               left->col, enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_int_div(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    if (left->type == right->type)
    {
        switch (left->type)
        {
        case AST_INT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = left->int_val / right->int_val;
            return ret;
        }
        case AST_FLOAT:
        {
            AST_T *ret = init_ast(AST_INT, left->line, left->col);
            ret->int_val = (int)left->float_val / (int)right->float_val;
            return ret;
        }

        default:
            printf("%d:%d -- Cannot divide objects of type '%s'\n",
                   left->line,
                   left->col,
                   enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_INT, left->line, left->col);
        ret->int_val = (int)left->float_val / right->int_val;
        return ret;
    }
    else if (left->type == AST_INT && right->type == AST_FLOAT)
    {
        AST_T *ret = init_ast(AST_INT, left->line, left->col);
        ret->int_val = left->int_val / (int)right->float_val;
        return ret;
    }
    else
    {
        printf("%d:%d -- Cannot divide objects of type '%s' and '%s'\n",
               left->line,
               left->col,
               enum_names[left->type],
               enum_names[right->type]);
        exit(1);
    }
}

AST_T *ast_visit_if_statement(AST_T *node)
{
    AST_T *pred = ast_visit(node->if_predicate);
    if (pred->is_true > 0)
        return ast_visit(node->if_body);
    else if (node->else_body)
        return ast_visit(node->else_body);
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *ast_visit_ret_statement(AST_T *node)
{
    return ast_visit(node->return_expr);
}

AST_T *ast_visit_compound(AST_T *node)
{
    for (int i = 0; i < node->compound_size; i++)
    {
        if (node->compound_val[i]->type == AST_RET_STMNT)
            return __visit_ret_stmnt__(node, node->compound_val[i]);

        AST_T *visit = ast_visit(node->compound_val[i]);
        if (visit->type == AST_RET_STMNT)
            return __visit_ret_stmnt__(node, visit);
    }
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *__visit_ret_stmnt__(AST_T *node, AST_T *to_visit)
{
    if (node->parent != NULL && node->parent->type != AST_FUNC_DEF)
    {
        AST_T *ret = init_ast(AST_RET_STMNT, to_visit->line, to_visit->col);
        ret->return_expr = ast_visit(to_visit);
        ret->scope = node->scope;
        ret->global_scope = node->global_scope;
        ret->parent = node;
        return ret;
    }
    else
        return ast_visit(to_visit);
}

AST_T *init_ast(int type, unsigned int line, unsigned int col)
{
    AST_T *ast = calloc(1, sizeof(struct AST_STRUCT));

    ast->scope = (void *)0;
    ast->global_scope = (void *)0;
    ast->type = type;
    ast->line = line;
    ast->col = col;

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

    // AST ARR & ARR DEF
    ast->arr = (void *)0;
    ast->arr_size_expr = (void *)0;
    ast->arr_size = 0;

    // AST ARR INDEX
    ast->arr_index = (void *)0;
    ast->arr_inner_index = (void *)0;

    // AST ARR INDEX ASSIGNMENT
    ast->arr_new_val = (void *)0;

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