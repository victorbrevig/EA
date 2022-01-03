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
      std::string_view fileContentView = fileContent;
      size_t offset = 0;
      std::string_view line;
      while (offset != std::string::npos && (line = GetNextLine(fileContentView, offset)) != "NODE_COORD_SECTION"); //Skip header

      while (offset != std::string::npos && (line = GetNextLine(fileContentView, offset)) != "EOF")
      {
        //Parse coordinates
        size_t firstWhiteSpace = line.find(' ');
        size_t firstChar = firstWhiteSpace + 1;
        size_t secondWhiteSpace = line.find(' ', firstChar);
        size_t secondChar = secondWhiteSpace + 1;
        if (firstWhiteSpace != std::string::npos && secondWhiteSpace != std::string::npos)
        {
          std::string_view xStr = line.substr(firstChar, secondChar - firstChar);
          std::string_view yStr = line.substr(secondChar);
          int x = 5;
        }
      }

      return Graph();
    }
  }
}