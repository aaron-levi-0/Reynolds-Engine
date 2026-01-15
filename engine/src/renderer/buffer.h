#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    TYPE_FLOAT,
    TYPE_FLOAT2,
    TYPE_FLOAT3,
    TYPE_FLOAT4,
    TYPE_MAT3
} bufferDataType;

struct bufferElement
{
    char* name;
    bufferDataType type;
    size_t size;
    size_t offset;
    bool normalised;
};

extern void setBufferLayout_default_impl(uint32_t );
extern void setBufferLayout_struct_impl(uint32_t , size_t );

// Overloaded function using _Generic
#define setBufferLayout(bufferID, ...) \
    _Generic((__VA_ARGS__), \
        size_t: setBufferLayout_struct_impl, \
        default: setBufferLayout_default_impl \
    )(bufferID, ##__VA_ARGS__)


extern void create_buffer_layout();
extern void push_buffer_element(const char* , bufferDataType , bool );
extern void push_struct_element(const char* , bufferDataType , bool , size_t );
extern void delete_buffer_layout();

extern uint32_t createVA();
extern uint32_t createVB(float* , uint32_t );
extern uint32_t createIB(uint32_t* , uint32_t );

extern void writeVertexBuffer(uint32_t , void* , uint32_t );
extern void drawTriangles(uint32_t , uint32_t );

extern void deleteVA(uint32_t );
extern void freeBuffer(uint32_t );

#endif