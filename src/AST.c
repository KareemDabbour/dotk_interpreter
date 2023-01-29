#include "include/AST.h"
#include "include/scope.h"
#include <stdio.h>
#include <string.h>

static AST_T *builtin_function_print(AST_T *node, AST_T **args, size_t args_size);
static AST_T *builtin_function_len(AST_T *node, AST_T **args, size_t args_size);
static AST_T *builtin_function_not(AST_T *node, AST_T **args, size_t args_size);
static AST_T *builtin_function_abs(AST_T *node, AST_T **args, size_t args_size);
static AST_T *builtin_function_range(AST_T *node, AST_T **args, size_t args_size);
static AST_T *__visit_ret_stmnt__(AST_T *node, AST_T *to_visit);
static AST_T *__visit_break_stmnt__(AST_T *node, AST_T *to_visit);
static AST_T *__visit_continue_stmnt__(AST_T *node, AST_T *to_visit);
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
static AST_T *ast_visit_while_loop(AST_T *node);
static AST_T *ast_visit_foreach_loop(AST_T *node);
static AST_T *ast_visit_ret_statement(AST_T *node);
static AST_T *ast_visit_break_statement(AST_T *node);
static AST_T *ast_visit_continue(AST_T *node);
static AST_T *ast_visit_and(AST_T *node);
static AST_T *ast_visit_or(AST_T *node);
static AST_T *ast_visit_not_eq_comp(AST_T *node);
static AST_T *ast_visit_eq_comp(AST_T *node);
static AST_T *ast_visit_lt_comp(AST_T *node);
static AST_T *ast_visit_gt_comp(AST_T *node);
static AST_T *ast_visit_lte_comp(AST_T *node);
static AST_T *ast_visit_gte_comp(AST_T *node);
static AST_T *ast_visit_type_cast(AST_T *node);
static AST_T *ast_visit_slice(AST_T *node);

char *file_path;

const char *cast_type_enum_names[6] = {
    "NONE",
    "FLOAT",
    "INT",
    "STR",
    "BOOLEAN",
    "LIST"};

const char *
    ast_enum_names[38] = {"NULL",
                          "FUNCTION",
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
                          "MODULUS",
                          "WHILE LOOP",
                          "BREAK STATEMENT",
                          "TYPE CAST",
                          "FOR EACH LOOP",
                          "CONTINUE",
                          "RANGE",
                          "SLICE"};

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
                printf("%ld, ", temp->int_val);
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
                printf("%s, ", ast_enum_names[temp->type]);
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
            printf("%ld", temp->int_val);
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
            printf("%s", ast_enum_names[temp->type]);
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
                sprintf(str_temp, "%ld, ", temp->int_val);
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
                sprintf(str_temp, "%s, ", ast_enum_names[temp->type]);
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
            sprintf(str_temp, "%ld", temp->int_val);
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
            sprintf(str_temp, "%s", ast_enum_names[temp->type]);
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
                                    printf(KRED);
                                    printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                                           file_path,
                                           x->line,
                                           x->col,
                                           ast_enum_names[x->type]);
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
                                printf(KRED);
                                printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
                                       file_path,
                                       x_arr_elem->line,
                                       x_arr_elem->col,
                                       ast_enum_names[x_arr_elem->type],
                                       ast_enum_names[y_arr_elem->type]);
                                exit(1);
                            }
                            if (ret == 0)
                                break;
                        }
                    }
                    break;
                }
                default:
                    printf(KRED);
                    printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                           file_path,
                           x_elem->line,
                           x_elem->col,
                           ast_enum_names[x_elem->type]);
                    printf(KNRM);
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
                printf(KRED);
                printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
                       file_path,
                       x_elem->line,
                       x_elem->col,
                       ast_enum_names[x_elem->type],
                       ast_enum_names[y_elem->type]);
                printf(KNRM);
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
        printf(KRED);
        printf("%s:%d:%d -- 'len()' takes exactly one argument (%ld given)\n",
               file_path,
               node->line,
               node->col,
               args_size);
        printf(KNRM);
        exit(1);
    }
    AST_T *ret = init_ast(AST_INT, node->line, node->col);
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    AST_T *visited_ast = ast_visit(args[0]);
    switch (visited_ast->type)
    {
    case AST_STR:
        ret->int_val = visited_ast->arr_size;
        break;
    case AST_ARR:
        ret->int_val = visited_ast->arr_size;
        break;
    case AST_RANGE:
        ret->int_val = visited_ast->range_size;
        break;
    default:
        printf(KRED);
        printf("%s:%d:%d -- Cannot get length of type '%s'.", file_path, node->line, node->col, ast_enum_names[visited_ast->type]);
        printf(KNRM);
        exit(1);
    }
    return ret;
}

AST_T *builtin_function_print(AST_T *node, AST_T **args, size_t args_size)
{
    if (args_size == 0)
    {
        printf(KYEL);
        printf("%s:%d:%d -- WARNING: Calling 'print()' with no args\n", file_path, node->line, node->col);
        printf(KNRM);
    }
    for (size_t i = 0; i < args_size; i++)
    {
        AST_T *visited_ast = ast_visit(args[i]);

        switch (visited_ast->type)
        {
        case AST_STR:
            printf("%s\n", visited_ast->str_val);
            break;
        case AST_INT:
            printf("%ld\n", visited_ast->int_val);
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
        case AST_RANGE:
        {
            printf("<Range Object at <%p>>\n", &node);
            break;
        }
        default:
            printf("%s\n", ast_enum_names[visited_ast->type]);
            break;
        }
    }
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *builtin_function_not(AST_T *node, AST_T **args, size_t args_size)
{
    if (args_size == 0 || args_size > 1)
    {
        printf(KRED);
        printf("%s:%d:%d -- 'not()' takes exactly one argument (%ld given)\n",
               file_path,
               node->line,
               node->col,
               args_size);
        printf(KNRM);
        exit(1);
    }
    AST_T *ret = init_ast(AST_BOOL, node->line, node->col);
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    AST_T *visited_ast = ast_visit(args[0]);
    ret->is_true = visited_ast->is_true ? 0 : 1;
    return ret;
}

AST_T *builtin_function_abs(AST_T *node, AST_T **args, size_t args_size)
{
    if (args_size == 0 || args_size > 1)
    {
        printf(KRED);
        printf("%s:%d:%d -- 'abs()' takes exactly one argument (%ld given)\n",
               file_path,
               node->line,
               node->col,
               args_size);
        printf(KNRM);
        exit(1);
    }
    AST_T *ret = init_ast(AST_NOOP, node->line, node->col);
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    AST_T *visted_ast = ast_visit(args[0]);
    switch (visted_ast->type)
    {
    case AST_INT:
        ret->type = AST_INT;
        ret->int_val = abs(visted_ast->int_val);
        break;
    case AST_FLOAT:
        ret->type = AST_FLOAT;
        ret->float_val = fabs(visted_ast->float_val);
        break;
    default:
        printf(KRED);
        printf("%s:%d:%d -- Cannot call 'abs()' with type %s can only be called on type INT or type FLOAT.\n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[visted_ast->type]);
        printf(KNRM);
        exit(1);
    }
    return ret;
}

AST_T *builtin_function_range(AST_T *node, AST_T **args, size_t args_size)
{
    if (args_size == 0 || args_size > 3)
    {
        printf(KRED);
        printf("%s:%d:%d -- 'range(end)', 'range(start, end)' or 'range(start, end, step)' takes one, two or three arguments (%ld given)\n",
               file_path,
               node->line,
               node->col,
               args_size);
        printf(KNRM);
        exit(1);
    }
    AST_T *start_ast = (void *)0;
    AST_T *end_ast = (void *)0;
    AST_T *step_ast = (void *)0;
    int start = 0;
    int end = 0;
    int step = 1;
    if (args_size == 1)
    {
        end_ast = ast_visit(args[0]);
        end = end_ast->int_val;
    }
    else if (args_size == 2)
    {
        start_ast = ast_visit(args[0]);
        end_ast = ast_visit(args[1]);
        start = start_ast->int_val;
        end = end_ast->int_val;
    }
    else if (args_size == 3)
    {
        start_ast = ast_visit(args[0]);
        end_ast = ast_visit(args[1]);
        step_ast = ast_visit(args[2]);
        start = start_ast->int_val;
        end = end_ast->int_val;
        step = step_ast->int_val;
    }

    if (start_ast != (void *)0 && (start_ast->type != AST_INT))
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot call 'range()' with start argument of type %s. Must be of type INT\n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[start_ast->type]);
        printf(KNRM);
        exit(1);
    }
    if ((end_ast->type != AST_INT))
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot call 'range()' with end argument of type %s. Must be of type INT\n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[end_ast->type]);
        printf(KNRM);
        exit(1);
    }
    if (step_ast != (void *)0 && (step_ast->type != AST_INT))
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot call 'range()' with step argument of type %s. Must be of type INT\n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[step_ast->type]);
        printf(KNRM);
        exit(1);
    }
    AST_T *ret = init_ast(AST_RANGE, node->line, node->col);
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    ret->parent = node->parent;
    if (start > end && step > 0)
        return ret;
    ret->range_start = start;
    ret->range_end = end;
    ret->range_step = step;
    ret->range_size = ceil((abs(start - end) + abs(step) - 1) / abs(step));
    return ret;
}

void ast_set_file_path(char *fp)
{
    file_path = fp;
}

AST_T *ast_visit(AST_T *node)
{
// For debugging
#if DEBUG
    printf(KCYN);
    printf("Visiting: %s\n", ast_enum_names[node->type]);
    printf(KNRM);
#endif

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
    case AST_WHILE_LOOP:
        return ast_visit_while_loop(node);
    case AST_FOR_EACH:
        return ast_visit_foreach_loop(node);
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
    case AST_BREAK_STMNT:
        return ast_visit_break_statement(node);
    case AST_CONTINUE:
        return ast_visit_continue(node);
    case AST_ARR:
        return ast_visit_arr(node);
    case AST_ARR_DEF:
        return ast_visit_arr_def(node);
    case AST_ARR_INDEX_ASSIGNMENT:
        return ast_visit_arr_index_assignment(node);
    case AST_ARR_INDEX:
        return ast_visit_arr_index(node);
    case AST_TYPE_CAST:
        return ast_visit_type_cast(node);
    case AST_RANGE:
        return node;
    case AST_SLICE:
        return ast_visit_slice(node);
    case AST_NOOP:
        return node;
    default:
        printf(KRED);
        printf("%s:%d:%d -- Uncaught statement of type: '%s'\n", file_path, node->line, node->col, ast_enum_names[node->type]);
        exit(1);
    }
}

int __remove__var_def(AST_T *node)
{
    int return_or_break = 0;
    for (int i = 0; i < node->compound_size; i++)
    {
        if (node->compound_val[i]->type == AST_BREAK_STMNT ||
            node->compound_val[i]->type == AST_RET_STMNT)
        {
            return_or_break = 1;
            break;
        }
        switch (node->compound_val[i]->type)
        {
        case AST_WHILE_LOOP:
        {
            AST_T *rew = node->compound_val[i];
            AST_T *t = ast_visit(node->compound_val[i]->while_predicate);
            if (t->is_true)
                if (__remove__var_def(node->compound_val[i]->loop_body))
                    return 1;
            break;
        }
        case AST_IF_STMNT:
        {
            if (ast_visit(node->compound_val[i]->if_predicate)->is_true)
            {
                if (node->compound_val[i]->if_body != NULL)
                    if (__remove__var_def(node->compound_val[i]->if_body))
                        return 1;
            }
            else
            {
                if (node->compound_val[i]->else_body != NULL)
                    if (__remove__var_def(node->compound_val[i]->else_body))
                        return 1;
            }
            break;
        }
        default:
            break;
        }
    }

    for (int i = 0; i < node->compound_size; i++)
    {
        if (node->compound_val[i]->type == AST_VAR_DEF)
            scope_rem_var_def(node->scope, node->compound_val[i]->var_def_var_name);
        if (node->compound_val[i]->type == AST_BREAK_STMNT || node->compound_val[i]->type == AST_RET_STMNT)
            return return_or_break;
    }
    return return_or_break;
}

AST_T *ast_visit_slice(AST_T *node)
{
    AST_T *left = ast_visit(node->op_left);
    AST_T *right = ast_visit(node->op_right);
    node->op_left->int_val = left->int_val;
    node->op_right->int_val = right->int_val;

    if (left->type != AST_INT && left->type != AST_NOOP)
    {
        printf(KRED);
        printf("%s:%d:%d -- Slice indices must be of type 'INT' or 'NULL': Type '%s' was used\n",
               file_path,
               node->op_left->line,
               node->op_left->col,
               ast_enum_names[left->type]);
        exit(1);
    }
    if (right->type != AST_INT && right->type != AST_NOOP)
    {
        printf(KRED);
        printf("%s:%d:%d -- Slice indices must be of type 'INT' or 'NULL': Type '%s' was used\n",
               file_path,
               node->op_right->line,
               node->op_right->col,
               ast_enum_names[right->type]);
        exit(1);
    }
    return node;
}

AST_T *ast_visit_func_call(AST_T *node)
{
    if (strncmp(node->func_call_name, "print", 6) == 0)
        return builtin_function_print(node, node->func_call_args, node->func_call_args_size);
    if (strncmp(node->func_call_name, "len", 4) == 0)
        return builtin_function_len(node, node->func_call_args, node->func_call_args_size);
    if (strncmp(node->func_call_name, "not", 4) == 0)
        return builtin_function_not(node, node->func_call_args, node->func_call_args_size);
    if (strncmp(node->func_call_name, "abs", 4) == 0)
        return builtin_function_abs(node, node->func_call_args, node->func_call_args_size);
    if (strncmp(node->func_call_name, "range", 6) == 0)
        return builtin_function_range(node, node->func_call_args, node->func_call_args_size);
    AST_T *func_def = scope_get_func_def(
        node->global_scope,
        node->func_call_name);
    if (func_def == NULL)
    {
        printf(KRED);
        printf("%s:%d:%d -- '%s' is not defined.\n",
               file_path,
               node->line,
               node->col,
               node->func_call_name);
        exit(1);
    }
    if (func_def->func_def_args_size != node->func_call_args_size)
    {
        printf(KRED);
        printf("%s:%d:%d -- '%s' expected %ld args but recieved %ld.\n",
               file_path,
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

    // // Getting the number of variables declared in the function body
    // int num_var_def_in_func = 0;
    // for (int i = 0; i < func_def->func_body->compound_size; i++)
    // {
    //     if (func_def->func_body->compound_val[i]->type == AST_VAR_DEF)
    //         num_var_def_in_func++;
    // }
    // // Keep track of the variables declared in the function body
    // char *vars_to_be_removed[num_var_def_in_func];
    // for (int i = 0; i < func_def->func_body->compound_size; i++)
    // {
    //     if (func_def->func_body->compound_val[i]->type == AST_VAR_DEF)
    //         vars_to_be_removed[i] = func_def->func_body->compound_val[i]->var_def_var_name;
    // }

    // Visit the function body
    AST_T *visited_body = ast_visit(func_def->func_body);

    // Remove variables declared in function body
    __remove__var_def(func_def->func_body);

    // Remove param variable declarations from the function body
    for (int i = 0; i < (int)node->func_call_args_size; i++)
        scope_rem_var_def(func_def->func_body->scope, func_def->func_def_args[i]->var_name);

    // for (int i = 0; i < num_var_def_in_func; i++)
    //     scope_rem_var_def(func_def->func_body->scope, vars_to_be_removed[i]);

    return visited_body;
}

AST_T *ast_visit_func_def(AST_T *node)
{
    scope_add_func_def(node->global_scope, node);
    return node;
}

AST_T *ast_visit_var_def(AST_T *node)
{
    AST_T *ret = init_ast(AST_VAR_DEF, node->line, node->col);
    ret->parent = node->parent;
    ret->scope = node->scope;
    ret->global_scope = node->global_scope;
    ret->var_def_expr = node->var_def_expr;
    ret->var_def_var_name = node->var_def_var_name;
    ret->var_def_val = ast_visit(ret->var_def_expr);
    scope_add_var_def(node->scope, ret);
    return ret->var_def_val;
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
        printf(KRED);
        printf("%s:%d:%d -- Can't create an array with size of type '%s' must be 'INT' \n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[size->type]);
        exit(1);
    }
    if (size->int_val < 0)
    {
        printf(KRED);
        printf("%s:%d:%d -- Can't create an array with size below 0\n",
               file_path,
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
            AST_T *null_ast = init_ast(AST_NOOP, node->line, node->col);
            for (int i = 0; i < ret->arr_size; i++)
                ret->arr[i] = null_ast;
        }
        else
        {
            AST_T *default_val = ast_visit(node->var_def_val);
            for (int i = 0; i < ret->arr_size; i++)
                ret->arr[i] = default_val;
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

AST_T *get_arr_slice(AST_T *array, AST_T *slice)
{

    slice->range_start = slice->op_left->int_val;
    slice->range_end = slice->op_right->int_val;

    if (slice->range_start < 0)
        slice->range_start = array->arr_size + slice->range_start;
    else if (slice->range_start > array->arr_size)
        slice->range_start = array->arr_size;
    if (slice->range_end < 0)
        slice->range_end = array->arr_size + slice->range_end < 0 ? 0 : array->arr_size + slice->range_end;

    if (slice->op_left->type == AST_NOOP || slice->range_start < 0)
        slice->range_start = 0;
    if (slice->op_right->type == AST_NOOP || slice->range_end > array->arr_size)
        slice->range_end = array->arr_size;
    AST_T *ret = init_ast(AST_ARR, slice->line, slice->col);
    ret->arr_size = slice->range_end - slice->range_start < 0 ? 0 : slice->range_end - slice->range_start;
    if (ret->arr_size > 0)
        ret->arr = calloc(ret->arr_size, sizeof(AST_T *));
    int index = 0;
    for (int i = slice->range_start; i < slice->range_end; i++)
    {
        ret->arr[index++] = array->arr[i];
    }
    return ret;
}

AST_T *ast_visit_arr_index(AST_T *node)
{
    AST_T *arr;
    if (node->arr == NULL)
    {
        AST_T *aux = scope_get_var_def(node->scope, node->var_def_var_name);
        AST_T *temp = node;
        while (aux == NULL && temp->parent != (void *)0)
        {
            aux = scope_get_var_def(temp->scope, node->var_def_var_name);
            temp = temp->parent;
        }
        if (aux == NULL)
        {
            printf(KRED);
            printf("%s:%d:%d -- Undefined variable '%s'\n",
                   file_path, node->line,
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
        printf(KRED);
        printf("%s:%d:%d -- '%s' type var '%s' is not subscriptable\n",
               file_path, node->line,
               node->col,
               ast_enum_names[arr->type],
               node->var_def_var_name);
        exit(1);
    }

    AST_T *ast_index = ast_visit(node->arr_index);
    if (ast_index->type != AST_INT && ast_index->type != AST_SLICE)
    {
        printf(KRED);
        printf("%s:%d:%d -- Can't index an array with type '%s' must be 'INT' or 'SLICE'\n",
               file_path,
               node->line,
               node->col,
               ast_enum_names[ast_index->type]);
        exit(1);
    }
    if (ast_index->type == AST_SLICE)
        return get_arr_slice(arr, ast_index);
    size_t index;
    if (ast_index->int_val < 0)
        index = arr->arr_size + ast_index->int_val;
    else
        index = ast_index->int_val;
    if (index < 0 || index >= arr->arr_size)
    {
        printf(KRED);
        printf("%s:%d:%d -- Index Out of Bounds Error. Array has size %ld but %ld was passed in\n",
               file_path, node->line,
               node->col,
               arr->arr_size,
               ast_index->int_val);
        exit(1);
    }
    AST_T *ret;
    if (arr->type == AST_ARR)
    {
        ret = arr->arr[index];
        if (node->arr_inner_index != NULL)
        {
            if (ret->type != AST_ARR)
            {
                printf(KRED);
                printf("%s:%d:%d -- Type '%s' is not subscriptable\n",
                       file_path, node->line,
                       node->col,
                       ast_enum_names[ret->type]);
                exit(1);
            }
            node->arr_inner_index->arr = ret->arr;
            node->arr_inner_index->arr_size = ret->arr_size;
            node->arr_inner_index->scope = node->scope;
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
        while (aux == NULL && temp->parent != (void *)0)
        {
            aux = scope_get_var_def(temp->scope, node->var_def_var_name);
            temp = temp->parent;
        }
        if (aux == NULL)
        {
            printf(KRED);
            printf("%s:%d:%d -- Undefined variable '%s'\n",
                   file_path, node->line,
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
        printf(KRED);
        printf("%s:%d:%d -- '%s' type var '%s' does not support item assignment \n",
               file_path, node->line,
               node->col,
               ast_enum_names[arr->type],
               node->var_def_var_name);
        exit(1);
    }
    AST_T *ast_index = ast_visit(node->arr_index);
    if (ast_index->type != AST_INT)
    {
        printf(KRED);
        printf("%s:%d:%d -- Can't index an array with type '%s' must be 'INT' \n",
               file_path, node->line,
               node->col,
               ast_enum_names[ast_index->type]);
        exit(1);
    }
    size_t index;
    if (ast_index->int_val < 0)
        index = arr->arr_size + ast_index->int_val;
    else
        index = ast_index->int_val;
    if (index < 0 || index >= arr->arr_size)
    {
        printf(KRED);
        printf("%s:%d:%d -- Index Out of Bounds Error '%s' has size %ld but %ld was passed in\n",
               file_path, node->line,
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
            printf(KRED);
            printf("%s:%d:%d -- '%s' type is not subscriptable\n",
                   file_path, node->line,
                   node->col,
                   ast_enum_names[inner_arr->type]);
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
    while (vardef == NULL && temp->parent != (void *)0)
    {
        vardef = scope_get_var_def(temp->scope, node->var_def_var_name);
        temp = temp->parent;
    }
    if (vardef == NULL)
    {
        printf(KRED);
        printf("%s:%d:%d -- Undefined variable '%s'\n",
               file_path,
               node->line,
               node->col,
               node->var_def_var_name);
        exit(1);
    }

    vardef->var_def_val = ast_visit(node->var_def_expr);
    scope_replace_var_def(vardef->scope, vardef);

    return ast_visit(vardef->var_def_val);
}

AST_T *ast_visit_var(AST_T *node)
{
    AST_T *vardef = scope_get_var_def(node->scope, node->var_name);
    AST_T *temp = node;
    while (vardef == NULL && temp->parent != (void *)0)
    {
        vardef = scope_get_var_def(temp->scope, node->var_name);
        temp = temp->parent;
    }
    if (vardef == NULL)
    {
        printf(KRED);
        printf("%s:%d:%d -- Undefined variable '%s'\n",
               file_path, node->line,
               node->col,
               node->var_name);
        exit(1);
    }
    return ast_visit(vardef->var_def_val);
}

AST_T *ast_visit_str(AST_T *node)
{
    if ((node->str_val != (void *)0) && node->arr_size != 0)
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
            int str_len = strlen(left->str_val) + strlen(right->str_val);
            char *str = calloc(str_len + 1, sizeof(char));
            strcpy(str, left->str_val);
            ret->str_val = strcat(str, right->str_val);
            ret->arr_size = str_len;
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot add objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            printf(KRED);
            printf("%s:%d:%d -- Can only concatenate LIST (not 'FLOAT') to LIST\n",
                   file_path, left->line,
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
            sprintf(str, "%ld%s", left->int_val, right->str_val);
            int len = strlen(str);
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            printf(KRED);
            printf("%s:%d:%d -- Can only concatenate LIST (not 'INT') to LIST\n",
                   file_path, left->line,
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
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_INT:
        {
            sprintf(str, "%s%ld", left->str_val, right->int_val);
            int len = strlen(str);
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_BOOL:
        {
            sprintf(str, "%s%s", left->str_val, right->is_true ? "True" : "False");
            int len = strlen(str);
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_NOOP:
        {
            sprintf(str, "%sNULL", left->str_val);
            int len = strlen(str);
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        case AST_ARR:
        {
            char *arr_str = get_arr_as_string(right);
            AST_T *ret = init_ast(AST_STR, left->line, left->col);
            int len = strlen(left->str_val) + strlen(arr_str);
            char *str = calloc(len + 1, sizeof(char));
            ret->arr_size = len;
            strcpy(str, left->str_val);
            ret->str_val = strcat(str, arr_str);
            free(arr_str);
            return ret;
        }
        case AST_RANGE:
        {
            sprintf(str, "%s<Range Object at <%p>>", left->str_val, &right);
            int len = strlen(str);
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
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
            ret->arr_size = len;
            ret->str_val = calloc(len + 1, sizeof(char));
            strncpy(ret->str_val, str, len);
            return ret;
        }
        printf(KRED);
        printf("%s:%d:%d -- Can only concatenate LIST (not '%s') to LIST\n",
               file_path, left->line,
               left->col,
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- bad operand type for unary operator '+': '%s'\n",
                   file_path, right->line,
                   right->col,
                   ast_enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (left->type == AST_RANGE && right->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR, left->line, left->col);
        char str[MAX_NUM_SPACE + MAX_STR_CHAR];
        sprintf(str, "<Range Object at <%p>>%s", &left, right->str_val);
        int len = strlen(str);
        ret->str_val = calloc(len + 1, sizeof(char));
        ret->arr_size = len;
        strncpy(ret->str_val, str, len);
        return ret;
    }
    else if (left->type == AST_BOOL && right->type == AST_STR)
    {
        AST_T *ret = init_ast(AST_STR, left->line, left->col);
        char str[MAX_NUM_SPACE + MAX_STR_CHAR];
        sprintf(str, "%s%s", left->is_true ? "True" : "False", right->str_val);
        int len = strlen(str);
        ret->arr_size = len;
        ret->str_val = calloc(len + 1, sizeof(char));
        strncpy(ret->str_val, str, len);
        return ret;
    }
    printf(KRED);
    printf("%s:%d:%d -- Cannot add objects of type '%s' and '%s'\n",
           file_path, left->line,
           left->col,
           ast_enum_names[left->type],
           ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot subtract objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- bad operand type for unary operator '-': '%s'\n",
                   file_path, right->line,
                   right->col,
                   ast_enum_names[right->type]);
            exit(1);
        }
        }
    }
    else if (right->type == AST_NOOP)
    {
        printf(KRED);
        printf("%s:%d:%d -- bad operand type for unary operator '-': '%s'\n",
               file_path, right->line,
               right->col,
               ast_enum_names[right->type]);
        exit(1);
    }
    else
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot subtract objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot perfom modulo on objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
            exit(1);
        }
        }
    }
    else
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot perfom modulo on objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot multiply objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot multiply objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
                printf(KRED);
                printf("%s:%d:%d -- Zero Division Error: Cannot Divide by 0.\n", file_path, right->line, right->col);
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
                printf(KRED);
                printf("%s:%d:%d -- Zero Division Error: Cannot Divide by 0.\n", file_path, right->line, right->col);
                exit(1);
            }
            ret->float_val = left->float_val / right->float_val;
            return ret;
        }

        default:
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
            exit(1);
        }
    }
    else if (left->type == AST_FLOAT && right->type == AST_INT)
    {
        AST_T *ret = init_ast(AST_FLOAT, left->line, left->col);
        if (right->int_val == 0)
        {
            printf(KRED);
            printf("%s:%d:%d -- Zero Division Error: Cannot Divide by 0.\n", file_path, right->line, right->col);
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
            printf(KRED);
            printf("%s:%d:%d -- Zero Division Error: Cannot Divide by 0.\n", file_path, right->line, right->col);
            exit(1);
        }
        ret->float_val = (float)left->int_val / right->float_val;
        return ret;
    }
    else
    {
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col, ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot compare objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot compare objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col, ast_enum_names[left->type],
               ast_enum_names[right->type]);
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
            printf(KRED);
            printf("%s:%d:%d -- Cannot divide objects of type '%s'\n",
                   file_path, left->line,
                   left->col,
                   ast_enum_names[left->type]);
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
        printf(KRED);
        printf("%s:%d:%d -- Cannot divide objects of type '%s' and '%s'\n",
               file_path, left->line,
               left->col,
               ast_enum_names[left->type],
               ast_enum_names[right->type]);
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

AST_T *ast_visit_while_loop(AST_T *node)
{
    AST_T *pred = ast_visit(node->while_predicate);
    if (pred->is_true > 0)
    {
        AST_T *ret = ast_visit(node->loop_body);
        if (ret->type == AST_BREAK_STMNT)
            return init_ast(AST_NOOP, node->line, node->col);
        else if (ret->type == AST_RET_STMNT)
            return ret;
        else
            return ast_visit(node);
    }
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *ast_visit_foreach_loop(AST_T *node)
{
    AST_T *ast_for_array = node->for_each_arr;
    if (ast_for_array == ((void *)0))
    {
        ast_for_array = ast_visit(node->for_each_arr_expr);
        if (ast_for_array->type != AST_ARR && ast_for_array->type != AST_RANGE)
        {
            printf(KRED);
            printf("%s:%d:%d -- Cannot iterate over none iterable type %s\n",
                   file_path, node->line,
                   node->col,
                   ast_enum_names[ast_for_array->type]);
            printf(KNRM);
            exit(1);
        }
        node->for_each_arr = ast_for_array;
    }
    if (node->for_each_arr->type == AST_RANGE)
    {
        if (node->for_each_index < ast_for_array->range_size)
        {
            AST_T *int_val = init_ast(AST_INT, node->line, node->col);
            int_val->int_val = ast_for_array->range_start + (node->for_each_index++ * ast_for_array->range_step);
            int_val->scope = node->scope;
            int_val->global_scope = node->global_scope;
            int_val->parent = node->parent;
            node->for_each_var->var_def_expr = int_val;
            ast_visit(node->for_each_var);
            AST_T *ret = ast_visit(node->loop_body);
            if (ret->type == AST_BREAK_STMNT)
            {
                node->for_each_index = 0;
                node->for_each_arr = ((void *)0);
                return init_ast(AST_NOOP, node->line, node->col);
            }
            else if (ret->type == AST_RET_STMNT)
            {
                node->for_each_index = 0;
                node->for_each_arr = ((void *)0);
                return ret;
            }
            else
                return ast_visit(node);
        }
    }
    else
    {
        if (node->for_each_index < ast_for_array->arr_size)
        {
            node->for_each_var->var_def_expr = ast_for_array->arr[node->for_each_index++];
            ast_visit(node->for_each_var);
            AST_T *ret = ast_visit(node->loop_body);
            if (ret->type == AST_BREAK_STMNT)
            {
                node->for_each_index = 0;
                node->for_each_arr = ((void *)0);
                return init_ast(AST_NOOP, node->line, node->col);
            }
            else if (ret->type == AST_RET_STMNT)
            {
                node->for_each_index = 0;
                node->for_each_arr = ((void *)0);
                return ret;
            }
            else
                return ast_visit(node);
        }
    }
    node->for_each_index = 0;
    node->for_each_arr = ((void *)0);
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *ast_visit_ret_statement(AST_T *node)
{
    return ast_visit(node->return_expr);
}

AST_T *ast_visit_type_cast(AST_T *node)
{
    AST_T *ret;
    node->var_def_val = ast_visit(node->var_def_expr);
    switch (node->var_def_val->type)
    {
    case AST_NOOP:
    {
        switch (node->cast_type)
        {
        case TO_INT:
        {
            ret = init_ast(AST_INT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            return ret;
        }
        case TO_FLOAT:
        {
            ret = init_ast(AST_FLOAT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            return ret;
        }
        case TO_STR:
        {
            ret = init_ast(AST_STR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->str_val = "";
            return ret;
        }
        case TO_BOOL:
        {
            ret = init_ast(AST_BOOL, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            return ret;
        }
        case TO_ARR:
        {
            ret = init_ast(AST_ARR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            return ret;
        }
        default:
            break;
        }
        break;
    }
    case AST_BOOL:
    {
        switch (node->cast_type)
        {
        case TO_INT:
        {
            ret = init_ast(AST_INT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->int_val = node->var_def_val->is_true;
            return ret;
        }
        case TO_FLOAT:
        {
            ret = init_ast(AST_FLOAT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->float_val = (float)node->var_def_val->is_true;
            return ret;
        }
        case TO_STR:
        {
            ret = init_ast(AST_STR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->str_val = node->var_def_val->is_true ? "True" : "False";
            ret->arr_size = node->var_def_val->is_true ? 5 : 6;
            return ret;
        }
        case TO_BOOL:
        {
            node->var_def_val->scope = node->scope;
            node->parent = node->parent;
            return node->var_def_val;
        }
        default:
            break;
        }
        break;
    }
    case AST_INT:
    {
        switch (node->cast_type)
        {
        case TO_INT:
        {
            node->var_def_val->scope = node->scope;
            node->parent = node->parent;
            return node->var_def_val;
        }
        case TO_FLOAT:
        {
            ret = init_ast(AST_FLOAT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->float_val = (float)node->var_def_val->int_val;
            return ret;
        }
        case TO_BOOL:
        {
            ret = init_ast(AST_BOOL, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->is_true = node->var_def_val->int_val ? 1 : 0;
            return ret;
        }
        case TO_STR:
        {
            ret = init_ast(AST_STR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->str_val = calloc(MAX_NUM_SPACE, sizeof(char));
            sprintf(ret->str_val, "%ld", node->var_def_val->int_val);
            ret->arr_size = strlen(ret->str_val);
            return ret;
        }
        default:
            break;
        }
        break;
    }
    case AST_FLOAT:
    {
        switch (node->cast_type)
        {
        case TO_INT:
        {
            ret = init_ast(AST_INT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->int_val = node->var_def_val->float_val;
            return ret;
        }
        case TO_FLOAT:
        {
            node->var_def_val->scope = node->scope;
            node->parent = node->parent;
            return node->var_def_val;
        }
        case TO_BOOL:
        {
            ret = init_ast(AST_BOOL, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->is_true = node->var_def_val->float_val ? 1 : 0;
            return ret;
        }
        case TO_STR:
        {
            ret = init_ast(AST_STR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->str_val = calloc(MAX_NUM_SPACE, sizeof(char));
            sprintf(ret->str_val, "%.2f", node->var_def_val->float_val);
            ret->arr_size = strlen(ret->str_val);
            return ret;
        }
        default:
            break;
        }
        break;
    }
    case AST_ARR:
    {
        if (node->cast_type == TO_STR)
        {
            ret = init_ast(AST_STR, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->str_val = get_arr_as_string(node->var_def_val);
            ret->arr_size = strlen(ret->str_val);
            return ret;
        }
        else if (node->cast_type == TO_BOOL)
        {
            ret = init_ast(AST_BOOL, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->is_true = node->var_def_val->arr_size ? 1 : 0;
            return ret;
        }
        else if (node->cast_type == TO_ARR)
        {
            node->var_def_val->scope = node->scope;
            node->parent = node->parent;
            return node->var_def_val;
        }
        break;
    }
    case AST_STR:
    {
        switch (node->cast_type)
        {
        case TO_INT:
        {
            char *to_conv = node->var_def_val->str_val;
            char *p = to_conv;
            errno = 0;
            long int val = strtol(to_conv, &p, 10);
            if ((errno != 0) || (to_conv == p) || (*p != 0))
            {
                printf(KRED);
                printf("%s:%d:%d -- Invalid literal for int() with base 10: '%s'\n", file_path, node->line, node->col, node->var_def_val->str_val);
                exit(1);
            }
            ret = init_ast(AST_INT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->int_val = val;
            return ret;
        }
        case TO_FLOAT:
        {
            char *to_conv = node->var_def_val->str_val;
            char *p = to_conv;
            errno = 0;
            float val = strtof(to_conv, &p);
            if ((errno != 0) || (to_conv == p) || (*p != 0))
            {
                printf(KRED);
                printf("%s:%d:%d -- Could not convert STR to FLOAT: '%s'\n", file_path, node->line, node->col, node->var_def_val->str_val);
                exit(1);
            }
            ret = init_ast(AST_FLOAT, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            ret->float_val = val;
            return ret;
        }
        case TO_BOOL:
        {
            ret = init_ast(AST_BOOL, node->var_def_expr->line, node->var_def_expr->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            if (node->var_def_val->str_val == NULL)
                ret->is_true = 0;
            else if (strncmp(node->var_def_val->str_val, "True", 5) == 0)
                ret->is_true = 1;
            else if (strncmp(node->var_def_val->str_val, "False", 6) == 0)
                ret->is_true = 0;
            else
                ret->is_true = node->var_def_val->arr_size ? 1 : 0;
            return ret;
        }
        case TO_ARR:
        {
            ret = init_ast(AST_ARR, node->line, node->col);
            ret->scope = node->scope;
            ret->parent = node->parent;
            if (node->var_def_val->str_val != NULL)
            {
                ret->arr_size = strlen(node->var_def_val->str_val);
                ret->arr = calloc(ret->arr_size, sizeof(struct AST_STRUCT *));
                for (int i = 0; i < ret->arr_size; i++)
                {
                    ret->arr[i] = init_ast(AST_STR, node->line, node->col);
                    ret->arr[i]->arr_size = 1;
                    ret->arr[i]->str_val = calloc(2, sizeof(char));
                    strncpy(ret->arr[i]->str_val, &node->var_def_val->str_val[i], 1);
                    ret->arr[i]->str_val[1] = '\0';
                }
            }
            return ret;
        }
        case TO_STR:
        {
            node->var_def_val->scope = node->scope;
            node->parent = node->parent;
            return node->var_def_val;
        }
        default:
            break;
        }
    }
    case AST_RANGE:
    {
        if (node->cast_type == TO_ARR)
        {
            ret = init_ast(AST_ARR, node->line, node->col);
            ret->arr_size = node->var_def_val->range_size;
            ret->arr = calloc(ret->arr_size, sizeof(struct AST_STRUCT *));
            int val = node->var_def_val->range_start;
            for (int i = 0; i < ret->arr_size; i++)
            {
                ret->arr[i] = init_ast(AST_INT, node->line, node->col);
                ret->arr[i]->int_val = val;
                val += node->var_def_val->range_step;
                ret->arr[i]->scope = node->scope;
                ret->arr[i]->global_scope = node->global_scope;
                ret->arr[i]->parent = node->parent;
            }
            return ret;
        }
        break;
    }
    default:
        break;
    }
    printf(KRED);
    printf("%s:%d:%d -- Type '%s' cannot be converted to %s\n", file_path, node->var_def_expr->line, node->var_def_expr->col, ast_enum_names[node->var_def_val->type], cast_type_enum_names[node->cast_type]);
    exit(1);
}

AST_T *ast_visit_break_statement(AST_T *node)
{
    return node;
}

AST_T *ast_visit_continue(AST_T *node)
{
    return node;
}

AST_T *ast_visit_compound(AST_T *node)
{
    AST_T *ret;
    for (int i = 0; i < node->compound_size; i++)
    {
        // is the AST I'm currently looking at a return statement?
        if (node->compound_val[i]->type == AST_RET_STMNT)
        {
            ret = __visit_ret_stmnt__(node, node->compound_val[i]);
            if (ret)
                return ret;
            continue;
        }
        if (node->compound_val[i]->type == AST_CONTINUE)
        {
            if (__visit_continue_stmnt__(node, node->compound_val[i]))
                return node->compound_val[i];
            continue;
        }
        if (node->compound_val[i]->type == AST_BREAK_STMNT)
        {
            if (__visit_break_stmnt__(node, node->compound_val[i]))
                return node->compound_val[i];
            continue;
        }

        AST_T *visit = ast_visit(node->compound_val[i]);
        if (visit->type == AST_RET_STMNT)
        {
            ret = __visit_ret_stmnt__(node, visit);
            if (ret)
                return ret;
            continue;
        }
        if (visit->type == AST_BREAK_STMNT)
        {
            if (__visit_break_stmnt__(node, visit))
                return visit;
            continue;
        }
        if (visit->type == AST_CONTINUE)
        {
            if (__visit_continue_stmnt__(node, visit))
                return visit;
        }
    }
    return init_ast(AST_NOOP, node->line, node->col);
}

AST_T *__visit_break_stmnt__(AST_T *node, AST_T *to_visit)
{
    if (node->parent != NULL)
        return ast_visit(to_visit);
    else
    {
        printf(KYEL);
        printf("%s:%d:%d -- WARNING: 'break' outside of loop.\n", file_path, to_visit->line, to_visit->col);
        printf(KNRM);
        return NULL;
    }
}

AST_T *__visit_continue_stmnt__(AST_T *node, AST_T *to_visit)
{
    if (node->parent != NULL)
        return ast_visit(to_visit);
    else
    {
        printf(KYEL);
        printf("%s:%d:%d -- WARNING: 'continue' outside of loop.\n", file_path, to_visit->line, to_visit->col);
        printf(KNRM);
        return NULL;
    }
}

AST_T *__visit_ret_stmnt__(AST_T *node, AST_T *to_visit)
{
    if (node->parent != NULL)
    {
        // Is the return nested?
        if (node->parent->type != AST_FUNC_DEF)
        {
            AST_T *ret = init_ast(AST_RET_STMNT, to_visit->line, to_visit->col);
            ret->return_expr = ast_visit(to_visit);
            ret->scope = node->scope;
            ret->global_scope = node->global_scope;
            ret->parent = node;
            return ret;
        }
        // Return is not nested. Visit its return expression;
        else
            return ast_visit(to_visit);
    }
    else
    {
        printf(KYEL);
        printf("%s:%d:%d -- WARNING: 'return' outside function.\n", file_path, to_visit->line, to_visit->col);
        printf(KNRM);
        return NULL;
    }
}

AST_T *init_ast(int type, unsigned int line, unsigned int col)
{
    AST_T *ast = calloc(1, sizeof(struct AST_STRUCT));

    ast->cast_type = NONE;

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

    // AST WHILE
    ast->while_predicate = (void *)0;

    // AST BOOL
    ast->is_true = 0;

    // AST STR
    ast->str_val = (void *)0;

    // AST INT
    ast->int_val = 0l;

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

    // AST RANGE
    ast->range_start = 0;
    ast->range_end = 0;
    ast->range_step = 1;
    ast->range_size = 0;

    // AST FOR EACH
    ast->for_each_var = (void *)0;
    ast->for_each_arr = (void *)0;
    ast->for_each_arr_expr = (void *)0;
    ast->for_each_index = 0;

    // loops
    ast->loop_body = (void *)0;
    return ast;
}