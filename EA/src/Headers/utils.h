#ifndef UTILS_H
#define UTILS_H

#include "pch.h"
#include "vec.h"
#include "matrix.h"
#include <random>

namespace Utils
{
  namespace Files
  {
    std::string StripDirFromFilePath(const std::string& filePath);
    std::string GetWorkingDirectory();
    std::string ReadFile(const std::string& filePath);
  }

  namespace Parser
  {
    double ConvertToReal(const std::string_view& str);
    std::string GetNextLine(const std::string& fileContent, size_t& offset);
    std::string_view GetNextLine(const std::string_view& fileContent, size_t& offset);
  }

  enum class SHADER_TYPE
  {
    VERTEX,
    FRAGMENT
  };

  namespace OpenGL
  {
    GLuint CreateShaderProgramFromFiles(const std::string& vertexShaderfilePath, const std::string& fragmentShaderfilePath);
    GLuint CreateShaderFromFile(const std::string& filePath, SHADER_TYPE shaderType);
  }

  namespace Random
  {
    extern std::mt19937 engine;
    unsigned int Get();
    unsigned int GetRange(unsigned int min, unsigned int max);
    std::pair<uint32_t, uint32_t> GetTwoDistinct(unsigned int min, unsigned int max);
    bool WithProbability(double p); //p must be [0:1]
  }
}
#endif