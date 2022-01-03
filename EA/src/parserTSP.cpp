#include "pch.h"
#include "parserTSP.h"
#include "utils.h"

namespace Utils
{
  namespace Parser
  {
    Graph ParseTSPGraph(const std::string& filePath)
    {
      std::string fileContent = Files::ReadFile(filePath);
      size_t offset = 0;
      std::string line = GetNextLine(fileContent, offset);
      std::string line2 = GetNextLine(fileContent, offset);
      std::cout << line << '\n';
      std::cout << line2 << '\n';
      return Graph();
    }
  }
}