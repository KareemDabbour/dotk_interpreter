#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
typedef struct VISITOR_STRUCT
{
} visitor_T;

visitor_T *init_visitor();

AST_T *visitor_visit(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_func_call(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_func_def(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_var_def(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_var(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_str(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_compound(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_int(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_bool(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_float(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_add(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_sub(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_mul(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_div(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_int_div(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_if_statement(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_ret_statement(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_and(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_or(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_not_eq_comp(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_eq_comp(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_lt_comp(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_gt_comp(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_lte_comp(visitor_T *visitor, AST_T *node);
AST_T *visitor_visit_gte_comp(visitor_T *visitor, AST_T *node);

#endif