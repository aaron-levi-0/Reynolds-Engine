#include "core/glx.h"

struct cache_entry {
    char* key;
    int value;
    struct cache_entry* next;
};

struct cache {
    int size;
    struct cache_entry** entries;
};

extern struct cache* create_cache(int size);
extern void cache_set(struct cache* c, const char* key, int value);
extern int cache_get(struct cache* c, const char* key);
