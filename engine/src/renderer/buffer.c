#include "renderer/buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

#include "utils/vector.h"
#include "../include/logging.h"

#define VEC_INIT_CAPACITY 4

struct bufferElement element;
Vector bufferLayout;

uint32_t total_size;

static uint32_t data_type_size(bufferDataType type)
{
    uint8_t float_size = sizeof(float);

    switch (type)
    {
        case TYPE_FLOAT:    return float_size;
        case TYPE_FLOAT2:   return 2 * float_size;
        case TYPE_FLOAT3:   return 3 * float_size;
        case TYPE_FLOAT4:   return 4 * float_size;
        case TYPE_MAT3:     return (3 * 3) * float_size;
    }

    ASSERT_FATAL(false, "@buffer: Unknown data type!");
}

//can't imagine a type with over 65 000 components
static uint16_t getComponentCount(bufferDataType type)
{
    switch (type)
    {
        case TYPE_FLOAT:   return 1;
        case TYPE_FLOAT2:  return 2;
        case TYPE_FLOAT3:  return 3;
        case TYPE_FLOAT4:  return 4;
        case TYPE_MAT3:    return 3; // 3 * float[3]
    }

    ASSERT_FATAL(false, "@buffer: Unknown data type!");
}

static int OpenGL_convert_type(bufferDataType type)
{
    switch (type)
    {
        case TYPE_FLOAT:    return GL_FLOAT;
        case TYPE_FLOAT2:   return GL_FLOAT;
        case TYPE_FLOAT3:   return GL_FLOAT;
        case TYPE_FLOAT4:   return GL_FLOAT;
        case TYPE_MAT3:     return GL_FLOAT;
    }

    ASSERT_FATAL(false, "@buffer: Unknown data type!");
}

void create_buffer_layout()
{
    vector_init(&bufferLayout, sizeof(struct bufferElement), VEC_INIT_CAPACITY);
}

void push_buffer_element(const char* name, bufferDataType type, bool normalisation)
{   
    uint8_t name_size = strlen(name) + 1;
    element.name = malloc(name_size);
    ASSERT_FATAL(element.name, "@buffer: Failed to allocate memory for buffer element name.");
    
    strncpy(element.name, name, name_size);
    
    element.type        = type;
    element.size        = data_type_size(type);
    element.offset      = 0;
    element.normalised  = normalisation;

    vector_push_back(&bufferLayout, &element);
}

void push_struct_element(const char* name, bufferDataType type, bool normalisation, size_t offset)
{   
    uint8_t name_size = strlen(name) + 1;
    element.name = malloc(name_size);
    ASSERT_FATAL(element.name, "@buffer: Failed to allocate memory for buffer element name.");
    
    strncpy(element.name, name, name_size);
    
    element.type        = type;
    element.size        = data_type_size(type);
    element.offset      = offset;
    element.normalised  = normalisation;

    vector_push_back(&bufferLayout, &element);
}

static void CalculateOffsetsAndStride()
{
    size_t offset = 0;
    struct bufferElement* element_ptr = (struct bufferElement* )vector_at(&bufferLayout, 0);
    
    for (size_t i = 0; i < vector_size(&bufferLayout); i++) 
    {
        element_ptr -> offset = offset;
        offset += element_ptr -> size;

        REYNOLDS_DEBUG("@buffer: Element: %s,\tOffset: %zu bytes", element_ptr -> name, element_ptr -> offset);
        element_ptr++;
    }

    total_size += offset;
    REYNOLDS_DEBUG("@buffer: Total buffer size per vertex: %d bytes", total_size);
}

static void bind_buffer_layout(uint32_t bufferID, size_t size)
{
    glBindVertexArray(bufferID);
    struct bufferElement* element_ptr = (struct bufferElement* )vector_at(&bufferLayout, 0);

    for (size_t i = 0; i < vector_size(&bufferLayout); i++) 
    {
        glEnableVertexArrayAttrib(bufferID, i);
        glVertexAttribPointer(i, 
            getComponentCount(element_ptr -> type), 
            OpenGL_convert_type(element_ptr -> type), 
            element_ptr -> normalised ? GL_TRUE : GL_FALSE, 
            size, 
            (const void*)(uintptr_t) element_ptr -> offset);

        element_ptr++;
    }
}

void setBufferLayout_default_impl(uint32_t bufferID)
{
    size_t total_size = 0;

    CalculateOffsetsAndStride();
    bind_buffer_layout(bufferID, total_size);
}

void setBufferLayout_struct_impl(uint32_t bufferID, size_t size)
{
    bind_buffer_layout(bufferID, size);
}

void delete_buffer_layout()
{
    total_size = 0;

    for (size_t i = 0; i < vector_size(&bufferLayout); i++) 
    {
        struct bufferElement* element_ptr = (struct bufferElement* )vector_at(&bufferLayout, i);
        free(element_ptr -> name);
        element_ptr -> name = NULL;
    }

    free_vector(&bufferLayout);
}

uint32_t createVA()
{
    uint32_t buffer;

    glCreateVertexArrays(1, &buffer);
    glBindVertexArray(buffer);

    return buffer;
}

uint32_t createVB(float* verticies, uint32_t size)
{
    uint32_t buffer;
   
    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);  //UNCLEAR: shouldnt glCreate already bind the buffer? what would be the difference
                                            //between this and glGen
    //Creates GPU buffer for verticies
    glBufferData(GL_ARRAY_BUFFER, size, verticies, GL_DYNAMIC_DRAW);

    //TO-DO: abstract out opengl dependence
    /*
    switch(getRenderAPI())
    {
        case renderAPI.NONE:    ASSERT_LOG(false, "No render API selected!");
        case renderAPI.OPENGL:  return OpenGLVertexBuffer(verticies, size)
    }
    */

   return buffer;
}

void writeVertexBuffer(unsigned int buffer, void* data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

    //printf("@buffer: Total buffer size this draw call: %d bytes\n", size);
}

uint32_t createIB(uint32_t* indicies, uint32_t size)
{
    uint32_t buffer;

    glCreateBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicies, GL_STATIC_DRAW);

    return buffer;
}

void drawTriangles(uint32_t arrayID, uint32_t count)
{
    glBindVertexArray(arrayID);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL);
}

void freeBuffer(uint32_t bufferID)
{
    glDeleteBuffers(1, &bufferID);
}

void deleteVA (uint32_t arrayID)
{
    glDeleteVertexArrays(1, &arrayID);
}