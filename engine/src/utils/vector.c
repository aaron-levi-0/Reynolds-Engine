#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/logging.h"

#define VEC_GROWTH_FACTOR 2
#define VEC_SHRINK_FACTOR 0.5

void vector_init(Vector* vec, size_t elem_size, size_t initial_capacity) 
{
    ASSERT_LOG(vec, "Must provide a valid vector pointer");
    vec -> data = malloc(initial_capacity * elem_size);
    ASSERT_LOG(vec-> data, "Memory allocation failed");

    vec -> size = 0;
    vec -> capacity = initial_capacity;
    vec -> elem_size = elem_size;
}

void vector_resize(Vector* vec) 
{
	/*
		Resize the vector to a new capacity.
	*/

    if (vec-> size >= vec-> capacity) 
	{
        vec-> capacity *= VEC_GROWTH_FACTOR;    //TO-DO: Investigate ways to grow vector, maybe double until after 3 allocations then switch to 1.5 growth
        vec-> data = realloc(vec-> data, vec-> capacity * vec-> elem_size);

        if (!vec-> data) 
		{
            fprintf(stderr, "Memory reallocation failed!\n");
            return;
        }
    }
}

void vector_push_back(Vector* vec, const void* element) 
{
	/*
		Add an element to the end of the vector. Resize if necessary.
	*/
    if (vec-> size >= vec-> capacity)
        vector_resize(vec);
    
    // Copy the element into the array
    memcpy((char* )vec -> data + vec -> size * vec -> elem_size, element, vec -> elem_size);
    vec -> size++;
}

void* vector_at(const Vector* vec, size_t index) 
{
	/*
		Retrieve an element at a specific index.
	*/
    if (index >= vec -> size) {
        fprintf(stderr, "Index out of bounds!\n");
        return NULL;
    }
    return (char*)vec -> data + index * vec -> elem_size;
}

void vector_pop_back(Vector* vec) 
{
    if (vec-> size == 0) 
    {
        fprintf(stderr, "Vector is empty!\n");
        return;
    }
    vec->size--;    //NOTE: don't need to waste time actually removing vector, can just move size 'ptr'
}

size_t vector_size(const Vector* vec) {
    return vec-> size;
}

void free_vector(Vector* vec) 
{
	if(vec-> data)
		free(vec-> data);
	else 
		fprintf(stderr, "Attempting to free a vector that does not exist!\n");
	
    vec-> data = NULL;
    vec-> size = 0;
    vec-> capacity = 0;
}