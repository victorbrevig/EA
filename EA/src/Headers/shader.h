#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include "Utils.h"

class Shader
{
public:
  GLuint m_ID;
  Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
  void Use();
  ~Shader();

};

#endif