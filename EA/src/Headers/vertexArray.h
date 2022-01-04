#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include <glad/glad.h>
#include "VertexBuffer.h"

class VertexArray
{
public: 
  GLuint m_ID;
  VertexArray();
  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;
  ~VertexArray();
  void LinkAttribute(VertexBuffer& vertexBuffer, GLuint layout, GLuint components, GLenum type, GLsizei stride, void* offset);
  void Bind();
  void Unbind();

};

#endif