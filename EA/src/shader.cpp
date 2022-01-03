#include "pch.h"
#include "Shader.h"

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
{
  m_ID = Utils::OpenGL::CreateShaderProgramFromFiles(vertexShaderFilePath, fragmentShaderFilePath);
}

void Shader::Use()
{
  glUseProgram(m_ID);
}

Shader::~Shader()
{
  glDeleteProgram(m_ID);
}