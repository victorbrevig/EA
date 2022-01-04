#include "pch.h"
#include "VertexArray.h"

VertexArray::VertexArray()
{
  glGenVertexArrays(1, &m_ID);
}
void VertexArray::LinkAttribute(VertexBuffer& vertexBuffer, GLuint layout, GLuint components, GLenum type, GLsizei stride, void* offset)
{
  vertexBuffer.Bind();
  glVertexAttribPointer(layout, components, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(layout);
  vertexBuffer.Unbind();
}

void VertexArray::Bind()
{
  glBindVertexArray(m_ID);
}

void VertexArray::Unbind()
{
  glBindVertexArray(0);
}

void VertexArray::DeleteArrays()
{
  glDeleteVertexArrays(1, &m_ID);
}
