#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <glad/glad.h>

class VertexBuffer
{
public:
  GLuint m_ID;
  VertexBuffer(GLfloat* vertices, GLsizeiptr count);
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;
  void Bind();
  void Unbind();
  ~VertexBuffer();
};

#endif