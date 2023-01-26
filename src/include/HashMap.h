#ifndef HASHMAP_H
#define HASHMAP_H
#include "AST.h"

#define HASH_SIZE 1000

typedef struct HASHNODE_STRUCT
{
    char *key;
    int key_len;
    AST_T *value;
    struct HASHNODE_STRUCT *next;
} HashNode_T;

typedef struct HASHMAP_STRUCT
{
    HashNode_T *table[HASH_SIZE];
} HashMap_T;

HashMap_T *init_hash_map();

void map_put(HashMap_T *map, const char *key, AST_T *value);

AST_T *map_get(HashMap_T *map, const char *key);

void map_remove(HashMap_T *map, const char *key);

#endif