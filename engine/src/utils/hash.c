#include "hash.h"

// Creates a new cache of the given size
struct cache* create_cache(int size) {
    struct cache* c = malloc(sizeof(struct cache));
    c -> size = size;
    c -> entries = malloc(sizeof(struct cache_entry*) * size);
    for (int i = 0; i < size; i++)
        c -> entries[i] = NULL;
    
    return c;
}

// Hashes a key to an index in the cache
static int hash(struct cache* c, const char* key) {
    int sum = 0;
    for (size_t i = 0; i < strlen(key); i++)
        sum += key[i];
    
    return sum % c -> size;
}

// Adds a key-value pair to the cache
void cache_set(struct cache* c, const char* key, int value) {
    int index = hash(c, key);
    struct cache_entry* entry = c -> entries[index];
    while (entry) 
	{
        if (!strcmp(entry -> key, key)) 
		{
            // Update the value if the key already exists
            entry -> value = value;
            return;
        }
        entry = entry -> next;
    }
	
    // Create a new entry if the key doesn't exist
    struct cache_entry* new_entry = malloc(sizeof(struct cache_entry));
    new_entry -> key = strdup(key);
    new_entry -> value = value;
    new_entry -> next = c -> entries[index];
    c -> entries[index] = new_entry;
}

// Retrieves a value from the cache for a given key
int cache_get(struct cache* c, const char* key) {
    int index = hash(c, key);
    struct cache_entry* entry = c -> entries[index];
    while (entry) 
	{
        if (!strcmp(entry -> key, key) == 0)
            return entry -> value;
        
        entry = entry -> next;
    }
    return -1;
}
