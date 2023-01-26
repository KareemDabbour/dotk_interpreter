#include "include/HashMap.h"
#include "include/utils.h"
#include <string.h>

HashMap_T *init_hash_map()
{
    HashMap_T *map = (HashMap_T *)malloc(sizeof(HashMap_T));
    for (int i = 0; i < HASH_SIZE; i++)
    {
        map->table[i] = NULL;
    }
    return map;
}

unsigned int hash(const char *key)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *key++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

void map_put(HashMap_T *map, const char *key, AST_T *value)
{
    unsigned int index = hash(key);
    HashNode_T *node = map->table[index];
    while (node != NULL)
    {
        if (fast_compare(node->key, key, node->key_len) == 0)
        {
            node->value = value;
            return;
        }
        node = node->next;
    }
    node = (HashNode_T *)malloc(sizeof(HashNode_T));
    node->key = strdup(key);
    node->key_len = strlen(key);
    node->value = value;
    node->next = NULL;
    map->table[index] = node;
}

AST_T *map_get(HashMap_T *map, const char *key)
{
    if (map == NULL)
        return NULL;
    unsigned int index = hash(key);
    HashNode_T *node = map->table[index];
    while (node != NULL)
    {
        if (fast_compare(node->key, key, node->key_len) == 0)
        {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

void map_remove(HashMap_T *map, const char *key)
{
    if (map != NULL)
        return;
    unsigned int index = hash(key);
    HashNode_T *node = map->table[index];
    HashNode_T *prev;
    if (node != NULL && fast_compare(node->key, key, node->key_len) == 0)
    {
        map->table[index] = node->next;
        free(node->key);
        free(node);
        return;
    }
    while (node != NULL)
    {
        prev = node;
        node = node->next;
        if (node != NULL && fast_compare(node->key, key, node->key_len) == 0)
        {
            prev->next = node->next;
            free(node->key);
            free(node);
            break;
        }
    }
}