#include "include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void parser_eat(parser_T *parser, int token_type);

static AST_T *parser_parse_statements(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_expr(parser_T *parser, scope_T *scope, AST_T *parent);

static AST_T *parser_parse_factor(parser_T *parser, scope_T *scope, AST_T *parent);

static AST_T *parser_parse_term(parser_T *parser, scope_T *scope, AST_T *parent);

static AST_T *parser_parse_func_call(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_func_def(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_var(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_var_def(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_arr_index_assignment(parser_T *parser, scope_T *scope, char *var_name);

static AST_T *parser_parse_arr_index(parser_T *parser, scope_T *scope, char *var_name);

static AST_T *parser_parse_var_redef(parser_T *parser, scope_T *scope, char *var_name);

static AST_T *parser_parse_str(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_id(parser_T *parser, scope_T *scope);

static AST_T *parser_parse_continue(parser_T *parser, scope_T *scope);

const char *token_names[29] = {
    "IDENTIFIER",                       // 0
    "EQUALS",                           // 1
    "STRING",                           // 2
    "SEMI-COLON",                       // 3
    "LEFT PARENTHESIS",                 // 4
    "RIGHT PARENTHESIS",                // 5
    "LEFT BRACKET",                     // 6
    "RIGHT BRACKET",                    // 7
    "COMMA",                            // 8
    "End of File",                      // 9
    "ADDITION SYMBOL",                  // 10
    "MULTIPLICATION SYMBOL",            // 11
    "INT",                              // 12
    "FLOAT",                            // 13
    "SUBTRACTION SYMBOL",               // 14
    "DIVISION SYMBOL",                  // 15
    "INT DIVISION SYMBOL",              // 16
    "EQUALS COMPARATOR",                // 17
    "NOT EQUALS COMPARATOR",            // 18
    "LESS THAN COMPARATOR",             // 19
    "GREATER THAN COMPARATOR",          // 20
    "LESS THAN OR EQUAL COMPARATOR",    // 21
    "GREATER THAN OR EQUAL COMPARATOR", // 22
    "AND COMPARATOR",                   // 23
    "OR COMPARATOR",                    // 24
    "LEFT SQUARE BRACKET",              // 25
    "RIGHT SQUARE BRACKET",             // 26
    "MODULUS OPERATOR",                 // 27
    "COLON"                             // 28
};

parser_T *init_parser(lexer_T *lexer)
{
    parser_T *parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(parser->lexer);
    parser->prev_token = parser->current_token;
    parser->scope = init_scope();
    return parser;
}

void parser_eat(parser_T *parser, int token_type)
{
    if (parser->current_token->type == token_type)
    {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);
    }
    else
    {
        printf(KRED);
        printf("%s:%d:%d -- Unexpected Token: '%s' with type '%s'\n",
               parser->lexer->file_path,
               parser->current_token->line,
               parser->current_token->col,
               parser->current_token->value,
               token_names[parser->current_token->type]);
        printf("Expected: %s\n", token_names[token_type]);
        exit(1);
    }
}

AST_T *parser_parse(parser_T *parser, scope_T *scope)
{
    return parser_parse_statements(parser, scope);
}

AST_T *parser_parse_statements(parser_T *parser, scope_T *scope)
{
    int no_semi_continue = 0;
    AST_T *compound = init_ast(AST_COMPOUND, parser->current_token->line, parser->current_token->col);
    scope_T *new_scope = init_scope();
    compound->compound_val = calloc(1, sizeof(struct AST_STRUCT *));
    AST_T *ast_statement;
    ast_statement = parser_parse_expr(parser, new_scope, compound);
    if (ast_statement->type == AST_NOOP)
    {
        return ast_statement;
    }
    if ((ast_statement->type != AST_FUNC_DEF &&
         ast_statement->type != AST_IF_STMNT &&
         ast_statement->type != AST_WHILE_LOOP &&
         ast_statement->type != AST_FOR_EACH) ||
        parser->current_token->type == TOKEN_SEMI)
        parser_eat(parser, TOKEN_SEMI);
    else
        no_semi_continue = 1;
    ast_statement->scope = new_scope;
    compound->compound_val[0] = ast_statement;
    compound->compound_size += 1;

    while (((parser->prev_token->type == TOKEN_SEMI) && (parser->current_token->type != TOKEN_EOF)) || no_semi_continue)
    {
        no_semi_continue = 0;
        ast_statement = parser_parse_expr(parser, new_scope, compound);
        compound->compound_size += 1;
        ast_statement->parent = compound;

        compound->compound_val = realloc(
            compound->compound_val,
            compound->compound_size * sizeof(struct AST_STRUCT *));

        compound->compound_val[compound->compound_size - 1] = ast_statement;
        if (ast_statement->type == AST_NOOP)
            break;
        if ((ast_statement->type != AST_FUNC_DEF &&
             ast_statement->type != AST_IF_STMNT &&
             ast_statement->type != AST_WHILE_LOOP &&
             ast_statement->type != AST_FOR_EACH) ||
            parser->current_token->type == TOKEN_SEMI)
            parser_eat(parser, TOKEN_SEMI);
        else
            no_semi_continue = 1;
    }

    compound->global_scope = parser->scope;
    compound->scope = new_scope;
    return compound;
}

AST_T *parser_parse_expr(parser_T *parser, scope_T *scope, AST_T *parent)
{
    AST_T *left_node = parser_parse_term(parser, scope, parent);
    while ((parser->current_token->type == TOKEN_PLUS) ||
           (parser->current_token->type == TOKEN_SUB) ||
           (parser->current_token->type == TOKEN_AND) ||
           (parser->current_token->type == TOKEN_OR) ||
           (parser->current_token->type == TOKEN_MOD) ||
           (parser->current_token->type == TOKEN_COLON))
    {
        AST_T *temp = init_ast(AST_NOOP, parser->current_token->line, parser->current_token->col);
        switch (parser->current_token->type)
        {
        case TOKEN_SUB:
        {
            temp->type = AST_SUB;
            parser_eat(parser, TOKEN_SUB);
            break;
        }
        case TOKEN_PLUS:
        {
            temp->type = AST_ADD;
            parser_eat(parser, TOKEN_PLUS);
            break;
        }
        case TOKEN_AND:
        {
            temp->type = AST_AND;
            parser_eat(parser, TOKEN_AND);
            break;
        }
        case TOKEN_OR:
        {
            temp->type = AST_OR;
            parser_eat(parser, TOKEN_OR);
            break;
        }
        case TOKEN_MOD:
        {
            temp->type = AST_MOD;
            parser_eat(parser, TOKEN_MOD);
            break;
        }
        case TOKEN_COLON:
        {
            temp->type = AST_SLICE;
            parser_eat(parser, TOKEN_COLON);
            break;
        }
        default:
        {
            printf(KRED);
            printf("%s:%d:%d -- Unexpected token: %s \nExpected %s, %s, %s or %s\n",
                   parser->lexer->file_path,
                   parser->current_token->line,
                   parser->current_token->col,
                   token_names[parser->current_token->type],
                   token_names[TOKEN_PLUS],
                   token_names[TOKEN_SUB],
                   token_names[TOKEN_AND],
                   token_names[TOKEN_OR]);
            exit(1);
        }
        }
        temp->op_left = left_node;
        temp->op_right = parser_parse_term(parser, scope, parent);
        temp->op_right->parent = parent;
        left_node = temp;
    }
    left_node->global_scope = parser->scope;
    left_node->parent = parent;
    return left_node;
}

AST_T *parser_parse_term(parser_T *parser, scope_T *scope, AST_T *parent)
{
    AST_T *ast_node_left = parser_parse_factor(parser, scope, parent);

    while ((parser->current_token->type == TOKEN_MUL) ||
           (parser->current_token->type == TOKEN_DIV) ||
           (parser->current_token->type == TOKEN_INT_DIV) ||
           (parser->current_token->type == TOKEN_EQ_COMP) ||
           (parser->current_token->type == TOKEN_NEQ_COMP) ||
           (parser->current_token->type == TOKEN_LT_COMP) ||
           (parser->current_token->type == TOKEN_GT_COMP) ||
           (parser->current_token->type == TOKEN_LTE_COMP) ||
           (parser->current_token->type == TOKEN_GTE_COMP))
    {
        AST_T *temp = init_ast(AST_NOOP, parser->current_token->line, parser->current_token->col);
        switch (parser->current_token->type)
        {
        case TOKEN_MUL:
        {
            temp->type = AST_MUL;
            parser_eat(parser, TOKEN_MUL);
            break;
        }
        case TOKEN_INT_DIV:
        {
            temp->type = AST_INT_DIV;
            parser_eat(parser, TOKEN_INT_DIV);
            break;
        }
        case TOKEN_DIV:
        {
            temp->type = AST_DIV;
            parser_eat(parser, TOKEN_DIV);
            break;
        }
        case TOKEN_EQ_COMP:
        {
            temp->type = AST_EQ_COMP;
            parser_eat(parser, TOKEN_EQ_COMP);
            break;
        }
        case TOKEN_NEQ_COMP:
        {
            temp->type = AST_NEQ_COMP;
            parser_eat(parser, TOKEN_NEQ_COMP);
            break;
        }
        case TOKEN_LT_COMP:
        {
            temp->type = AST_LT_COMP;
            parser_eat(parser, TOKEN_LT_COMP);
            break;
        }
        case TOKEN_GT_COMP:
        {
            temp->type = AST_GT_COMP;
            parser_eat(parser, TOKEN_GT_COMP);
            break;
        }
        case TOKEN_LTE_COMP:
        {
            temp->type = AST_LTE_COMP;
            parser_eat(parser, TOKEN_LTE_COMP);
            break;
        }
        case TOKEN_GTE_COMP:
        {
            temp->type = AST_GTE_COMP;
            parser_eat(parser, TOKEN_GTE_COMP);
            break;
        }

        default:
            printf(KRED);
            printf("%s:%d:%d -- Unexpected token: '%s' with type '%d'\n",
                   parser->lexer->file_path,
                   parser->current_token->line,
                   parser->current_token->col,
                   parser->current_token->value,
                   parser->current_token->type);
            exit(1);
        }

        temp->op_left = ast_node_left;
        temp->op_right = parser_parse_factor(parser, scope, parent);
        ast_node_left = temp;
    }
    return ast_node_left;
}

AST_T *parser_parse_factor(parser_T *parser, scope_T *scope, AST_T *parent)
{
    switch (parser->current_token->type)
    {
    case TOKEN_FLOAT:
    {
        AST_T *ast_float = init_ast(AST_FLOAT, parser->current_token->line, parser->current_token->col);
        ast_float->float_val = strtof(parser->current_token->value, NULL);
        ast_float->parent = parent;
        parser_eat(parser, TOKEN_FLOAT);
        return ast_float;
        break;
    }
    case TOKEN_INT:
    {
        AST_T *ast_int = init_ast(AST_INT, parser->current_token->line, parser->current_token->col);
        ast_int->int_val = strtol(parser->current_token->value, NULL, 10);
        ast_int->parent = parent;
        parser_eat(parser, TOKEN_INT);
        return ast_int;
        break;
    }
    case TOKEN_LPAR:
    {
        parser_eat(parser, TOKEN_LPAR);
        AST_T *ast_expr = parser_parse_expr(parser, scope, parent);
        parser_eat(parser, TOKEN_RPAR);
        return ast_expr;
        break;
    }
    case TOKEN_LSBRA:
    {
        parser_eat(parser, TOKEN_LSBRA);
        AST_T *ret = init_ast(AST_ARR_DEF, parser->current_token->line, parser->current_token->col);
        ret->arr_size_expr = parser_parse_expr(parser, scope, parent);
        parser_eat(parser, TOKEN_RSBRA);
        AST_T *temp = ret;
        while (parser->current_token->type == TOKEN_LSBRA)
        {
            AST_T *inner_indexing = init_ast(AST_ARR_DEF, parser->current_token->line, parser->current_token->col);
            parser_eat(parser, TOKEN_LSBRA);
            inner_indexing->arr_size_expr = parser_parse_expr(parser, scope, inner_indexing);
            parser_eat(parser, TOKEN_RSBRA);
            inner_indexing->scope = scope;
            temp->arr_inner_index = inner_indexing;
            temp = temp->arr_inner_index;
        }
        if (parser->current_token->type == TOKEN_LBRA)
        {
            parser_eat(parser, TOKEN_LBRA);
            ret->var_def_val = parser_parse_expr(parser, scope, ret);
            parser_eat(parser, TOKEN_RBRA);
        }
        ret->scope = scope;
        ret->parent = parent;
        return ret;
        break;
    }
    case TOKEN_LBRA:
    {
        AST_T *arr = init_ast(AST_ARR_DEF, parser->current_token->line, parser->current_token->col);

        parser_eat(parser, TOKEN_LBRA);
        if (parser->current_token->type != TOKEN_RBRA)
        {
            arr->compound_val = calloc(1, sizeof(struct AST_STRUCT *));
            arr->compound_val[arr->arr_size] = parser_parse_expr(parser, scope, arr);
            arr->arr_size += 1;
            while (parser->current_token->type == TOKEN_COMMA)
            {
                parser_eat(parser, TOKEN_COMMA);
                arr->arr_size += 1;
                arr->compound_val = realloc(arr->compound_val, arr->arr_size * sizeof(struct AST_STRUCT *));
                arr->compound_val[arr->arr_size - 1] = parser_parse_expr(parser, scope, arr);
            }
        }
        arr->scope = scope;
        parser_eat(parser, TOKEN_RBRA);
        return arr;
    }
    case TOKEN_STR:
    {
        AST_T *ret1 = parser_parse_str(parser, scope);
        ret1->parent = parent;
        return ret1;
        break;
    }
    case TOKEN_ID:
    {
        AST_T *ret = parser_parse_id(parser, scope);
        ret->parent = parent;
        return ret;
        break;
    }
    default:
        return init_ast(AST_NOOP, parser->current_token->line, parser->current_token->col);
        break;
    }
}

AST_T *parser_parse_func_call(parser_T *parser, scope_T *scope)
{
    AST_T *ast_func_call = init_ast(AST_FUNC_CALL, parser->current_token->line, parser->current_token->col);
    ast_func_call->func_call_name = parser->prev_token->value;
    parser_eat(parser, TOKEN_LPAR);
    if (parser->current_token->type != TOKEN_RPAR)
    {
        ast_func_call->func_call_args = calloc(1, sizeof(struct AST_STRUCT *));
        AST_T *ast_expr = parser_parse_expr(parser, scope, ast_func_call);

        ast_func_call->func_call_args[0] = ast_expr;
        ast_func_call->func_call_args_size += 1;

        while (parser->current_token->type == TOKEN_COMMA)
        {
            parser_eat(parser, TOKEN_COMMA);
            AST_T *ast_expr = parser_parse_expr(parser, scope, ast_func_call);
            ast_func_call->func_call_args_size += 1;
            ast_func_call->func_call_args = realloc(
                ast_func_call->func_call_args,
                ast_func_call->func_call_args_size * sizeof(struct AST_STRUCT *));
            ast_func_call->func_call_args[ast_func_call->func_call_args_size - 1] = ast_expr;
        }
    }
    parser_eat(parser, TOKEN_RPAR);
    ast_func_call->scope = scope;
    ast_func_call->global_scope = parser->scope;
    return ast_func_call;
}

AST_T *parser_parse_func_def(parser_T *parser, scope_T *scope)
{
    AST_T *func_def = init_ast(AST_FUNC_DEF, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID);
    char *func_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID); // Func name

    parser_eat(parser, TOKEN_LPAR);
    if (parser->current_token->type != TOKEN_RPAR)
    {

        func_def->func_def_args = calloc(1, sizeof(struct AST_STRUCT *));

        AST_T *arg = parser_parse_var(parser, scope);
        func_def->func_def_args_size += 1;
        func_def->func_def_args[func_def->func_def_args_size - 1] = arg;

        while (parser->current_token->type == TOKEN_COMMA)
        {
            parser_eat(parser, TOKEN_COMMA);
            func_def->func_def_args_size += 1;
            func_def->func_def_args = realloc(
                func_def->func_def_args,
                func_def->func_def_args_size * sizeof(struct AST_STRUCT *));

            AST_T *arg = parser_parse_var(parser, scope);
            func_def->func_def_args[func_def->func_def_args_size - 1] = arg;
        }
    }
    parser_eat(parser, TOKEN_RPAR);

    parser_eat(parser, TOKEN_LBRA);
    func_def->func_body = parser_parse_statements(parser, scope);
    func_def->func_body->parent = func_def;
    func_def->func_name = func_name;

    parser_eat(parser, TOKEN_RBRA);
    func_def->scope = scope;
    func_def->global_scope = parser->scope;
    return func_def;
}

AST_T *parser_parse_if_stmnt(parser_T *parser, scope_T *scope)
{
    AST_T *ast_if = init_ast(AST_IF_STMNT, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID); // eat the 'IF'
    parser_eat(parser, TOKEN_LPAR);

    ast_if->if_predicate = parser_parse_expr(parser, scope, ast_if);

    parser_eat(parser, TOKEN_RPAR);

    parser_eat(parser, TOKEN_LBRA);

    ast_if->if_body = parser_parse_statements(parser, scope);
    ast_if->if_body->parent = ast_if;

    parser_eat(parser, TOKEN_RBRA);
    if (strncmp(parser->current_token->value, "else", 5) == 0)
    {
        parser_eat(parser, TOKEN_ID);
        parser_eat(parser, TOKEN_LBRA);
        ast_if->else_body = parser_parse_statements(parser, scope);
        ast_if->else_body->parent = ast_if;

        parser_eat(parser, TOKEN_RBRA);
    }
    ast_if->scope = scope;
    return ast_if;
}

AST_T *parser_parse_while_loop(parser_T *parser, scope_T *scope)
{
    AST_T *ast_while = init_ast(AST_WHILE_LOOP, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID); // eat the 'while'
    parser_eat(parser, TOKEN_LPAR);

    ast_while->while_predicate = parser_parse_expr(parser, scope, ast_while);

    parser_eat(parser, TOKEN_RPAR);

    parser_eat(parser, TOKEN_LBRA);

    ast_while->loop_body = parser_parse_statements(parser, scope);
    ast_while->loop_body->parent = ast_while;
    ast_while->scope = scope;
    ast_while->global_scope = parser->scope;
    parser_eat(parser, TOKEN_RBRA);
    return ast_while;
}

AST_T *parser_parse_foreach(parser_T *parser, scope_T *scope)
{
    AST_T *ast_foreach = init_ast(AST_FOR_EACH, parser->current_token->line, parser->current_token->col);
    ast_foreach->scope = scope;
    ast_foreach->global_scope = parser->scope;

    parser_eat(parser, TOKEN_ID); // Eat the 'for'
    parser_eat(parser, TOKEN_LPAR);

    ast_foreach->for_each_var = init_ast(AST_VAR_DEF, parser->current_token->line, parser->current_token->col);
    ast_foreach->for_each_var->var_def_var_name = parser->current_token->value;
    ast_foreach->for_each_var->scope = scope;
    ast_foreach->for_each_var->global_scope = parser->scope;
    parser_eat(parser, TOKEN_ID);

    parser_eat(parser, TOKEN_COLON);

    ast_foreach->for_each_arr_expr = parser_parse_expr(parser, scope, ast_foreach);
    parser_eat(parser, TOKEN_RPAR);

    parser_eat(parser, TOKEN_LBRA);

    ast_foreach->loop_body = parser_parse_statements(parser, scope);
    ast_foreach->loop_body->parent = ast_foreach;
    ast_foreach->scope = scope;
    ast_foreach->global_scope = parser->scope;
    parser_eat(parser, TOKEN_RBRA);
    return ast_foreach;
}

AST_T *parser_parse_return_stmnt(parser_T *parser, scope_T *scope)
{
    AST_T *ast_ret = init_ast(AST_RET_STMNT, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID); // eat the 'ret'
    ast_ret->return_expr = parser_parse_expr(parser, scope, ast_ret);

    ast_ret->scope = scope;
    ast_ret->global_scope = parser->scope;
    return ast_ret;
}

AST_T *parser_parse_break_stmnt(parser_T *parser, scope_T *scope)
{
    AST_T *ast_break = init_ast(AST_BREAK_STMNT, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID); // eat the 'break'
    ast_break->scope = scope;
    ast_break->global_scope = parser->scope;
    return ast_break;
}

AST_T *parser_parse_continue(parser_T *parser, scope_T *scope)
{
    AST_T *ast_continue = init_ast(AST_CONTINUE, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID); // eat the 'continue'
    ast_continue->scope = scope;
    ast_continue->global_scope = parser->scope;
    return ast_continue;
}

AST_T *parser_parse_var(parser_T *parser, scope_T *scope)
{
    char *var_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);
    if (parser->current_token->type == TOKEN_LPAR)
        return parser_parse_func_call(parser, scope);
    if (parser->current_token->type == TOKEN_EQ)
        return parser_parse_var_redef(parser, scope, var_name);
    if (parser->current_token->type == TOKEN_LSBRA)
        return parser_parse_arr_index_assignment(parser, scope, var_name);
    AST_T *ast_var = init_ast(AST_VAR, parser->current_token->line, parser->current_token->col);
    ast_var->var_name = var_name;
    ast_var->scope = scope;
    return ast_var;
}

AST_T *parser_parse_arr_index_assignment(parser_T *parser, scope_T *scope, char *var_name)
{
    AST_T *arr_index = parser_parse_arr_index(parser, scope, var_name);
    if (parser->current_token->type == TOKEN_EQ)
    {
        arr_index->type = AST_ARR_INDEX_ASSIGNMENT;
        AST_T *temp = arr_index->arr_inner_index;
        while (temp != NULL)
        {
            temp->type = AST_ARR_INDEX_ASSIGNMENT;
            temp = temp->arr_inner_index;
        }

        parser_eat(parser, TOKEN_EQ);
        arr_index->arr_new_val = parser_parse_expr(parser, scope, arr_index);
    }
    arr_index->scope = scope;
    arr_index->global_scope = parser->scope;
    return arr_index;
}
AST_T *parser_parse_arr_index(parser_T *parser, scope_T *scope, char *var_name)
{
    AST_T *arr_index = init_ast(AST_ARR_INDEX, parser->current_token->line, parser->current_token->col);
    arr_index->var_def_var_name = var_name;
    parser_eat(parser, TOKEN_LSBRA);
    arr_index->arr_index = parser_parse_expr(parser, scope, arr_index);
    parser_eat(parser, TOKEN_RSBRA);
    AST_T *temp = arr_index;
    while (parser->current_token->type == TOKEN_LSBRA)
    {
        AST_T *inner_indexing = init_ast(AST_ARR_INDEX, parser->current_token->line, parser->current_token->col);
        parser_eat(parser, TOKEN_LSBRA);
        inner_indexing->arr_index = parser_parse_expr(parser, scope, inner_indexing);
        parser_eat(parser, TOKEN_RSBRA);
        temp->arr_inner_index = inner_indexing;
        temp = temp->arr_inner_index;
    }
    return arr_index;
}

AST_T *parser_parse_var_redef(parser_T *parser, scope_T *scope, char *var_name)
{
    AST_T *var_redef = init_ast(AST_VAR_REDEF, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_EQ);
    AST_T *var_def_expr = parser_parse_expr(parser, scope, var_redef);

    var_redef->var_def_var_name = var_name;
    var_redef->var_def_expr = var_def_expr;

    var_redef->scope = scope;
    return var_redef;
}

AST_T *parser_parse_var_def(parser_T *parser, scope_T *scope)
{
    AST_T *var_def = init_ast(AST_VAR_DEF, parser->current_token->line, parser->current_token->col);
    parser_eat(parser, TOKEN_ID);
    char *var_def_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_EQ);
    AST_T *var_def_expr = parser_parse_expr(parser, scope, var_def);

    var_def->var_def_var_name = var_def_name;
    var_def->var_def_expr = var_def_expr;

    var_def->global_scope = parser->scope;
    var_def->scope = scope;
    return var_def;
}

AST_T *parser_parse_str(parser_T *parser, scope_T *scope)
{
    AST_T *ast_str = init_ast(AST_STR, parser->current_token->line, parser->current_token->col);

    ast_str->str_val = parser->current_token->value;
    ast_str->arr_size = strnlen(ast_str->str_val, MAX_STR_CHAR);
    parser_eat(parser, TOKEN_STR);
    ast_str->scope = scope;
    return ast_str;
}
AST_T *parser_parse_bool(parser_T *parser, scope_T *scope, int val)
{
    AST_T *ast_bool = init_ast(AST_BOOL, parser->current_token->line, parser->current_token->col);

    ast_bool->is_true = val;
    parser_eat(parser, TOKEN_ID);
    ast_bool->scope = scope;
    return ast_bool;
}

AST_T *parser_parse_type_cast(parser_T *parser, scope_T *scope, int type_cast)
{
    AST_T *ast_type_cast = init_ast(AST_TYPE_CAST, parser->current_token->line, parser->current_token->col);

    ast_type_cast->scope = scope;
    ast_type_cast->global_scope = parser->scope;

    ast_type_cast->cast_type = type_cast;
    parser_eat(parser, TOKEN_ID); // Eat the int, float or str
    parser_eat(parser, TOKEN_LPAR);
    ast_type_cast->var_def_expr = parser_parse_expr(parser, scope, ast_type_cast);
    parser_eat(parser, TOKEN_RPAR);

    return ast_type_cast;
}

AST_T *parser_parse_id(parser_T *parser, scope_T *scope)
{
    if (strncmp(parser->current_token->value, "var", 4) == 0)
    {
        return parser_parse_var_def(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "def", 4) == 0)
    {
        return parser_parse_func_def(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "if", 3) == 0)
    {
        return parser_parse_if_stmnt(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "while", 6) == 0)
    {
        return parser_parse_while_loop(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "return", 7) == 0)
    {
        return parser_parse_return_stmnt(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "int", 4) == 0)
    {
        return parser_parse_type_cast(parser, scope, TO_INT);
    }
    else if (strncmp(parser->current_token->value, "str", 4) == 0)
    {
        return parser_parse_type_cast(parser, scope, TO_STR);
    }
    else if (strncmp(parser->current_token->value, "float", 6) == 0)
    {
        return parser_parse_type_cast(parser, scope, TO_FLOAT);
    }
    else if (strncmp(parser->current_token->value, "bool", 5) == 0)
    {
        return parser_parse_type_cast(parser, scope, TO_BOOL);
    }
    else if (strncmp(parser->current_token->value, "list", 5) == 0)
    {
        return parser_parse_type_cast(parser, scope, TO_ARR);
    }
    else if (strncmp(parser->current_token->value, "break", 6) == 0)
    {
        return parser_parse_break_stmnt(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "continue", 9) == 0)
    {
        return parser_parse_continue(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "True", 5) == 0)
    {
        return parser_parse_bool(parser, scope, 1);
    }
    else if (strncmp(parser->current_token->value, "False", 6) == 0)
    {
        return parser_parse_bool(parser, scope, 0);
    }
    else if (strncmp(parser->current_token->value, "for", 4) == 0)
    {
        return parser_parse_foreach(parser, scope);
    }
    else if (strncmp(parser->current_token->value, "NULL", 5) == 0)
    {
        parser_eat(parser, TOKEN_ID);
        return init_ast(AST_NOOP, parser->current_token->line, parser->current_token->col);
    }
    else
    {
        return parser_parse_var(parser, scope);
    }
}
