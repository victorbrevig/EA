#include "pch.h"
#include "parserTSP.h"

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
    std::string GetNextLine(const std::string& fileContent, size_t& offset);
    std::string_view GetNextLine(const std::string_view& fileContent, size_t& offset);
  }
}