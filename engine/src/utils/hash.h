#ifndef HASH_H
#define HASH_H

#include "core/glx.h"

struct cache_entry {
    char* key;
    int data;
    struct cache_entry* next;
};

struct cache {
    int size;
    struct cache_entry** entries;
};

extern struct cache* create_cache(int size);
extern void cache_set(struct cache* c, const char* key, int data);
extern int cache_get(struct cache* c, const char* key);
extern void free_cache(struct cache* c);

#endif // HASH_H