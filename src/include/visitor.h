#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
typedef struct VISITOR_STRUCT
{
} visitor_T;

visitor_T *init_visitor();

AST_T *visitor_visit(visitor_T *visitor, AST_T *node);

#endif