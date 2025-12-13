#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>

typedef struct {
    void* data;       // Pointer to the array of elements
    size_t size;      // Number of elements in the vector
    size_t capacity;  // Total capacity of the vector
    size_t elem_size; // Size of each element in bytes
} Vector;

extern void vector_init(Vector* , size_t , size_t );
extern void vector_resize(Vector* );
extern void vector_push_back(Vector* , const void* );
extern void* vector_at(const Vector* , size_t );

extern size_t vector_size(const Vector* );
extern void vector_pop_back(Vector* );
extern void free_vector(Vector* );

#endif