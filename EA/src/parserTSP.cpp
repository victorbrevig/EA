#include "pch.h"
#include "parserTSP.h"
#include "utils.h"

namespace Utils
{
  namespace Parser
  {
    Graph ParseTSPGraph(const std::string& filePath)
    {
      Graph graph;
      std::string fileContent = Files::ReadFile(filePath);
      std::string_view fileContentView = fileContent;
      size_t offset = 0;
      std::string_view line;
      while (offset != std::string::npos && (line = GetNextLine(fileContentView, offset)) != "NODE_COORD_SECTION"); //Skip header

      while (offset != std::string::npos && (line = GetNextLine(fileContentView, offset)) != "EOF")
      {
        size_t firstOffset = 0;
        if (line.size() > 0 && line[0] == ' ')
        {
          while (line[firstOffset] == ' ') firstOffset++; //Skip initial white spaces
        }

        //Parse coordinates
        size_t firstWhiteSpace = line.find(' ', firstOffset);
        while (line[firstWhiteSpace + 1] == ' ') firstWhiteSpace++;
        size_t firstChar = firstWhiteSpace + 1;

        size_t secondWhiteSpace = line.find(' ', firstChar);
        while (line[secondWhiteSpace + 1] == ' ') secondWhiteSpace++;
        size_t secondChar = secondWhiteSpace + 1;

        if (firstWhiteSpace != std::string::npos && secondWhiteSpace != std::string::npos)
        {
          std::string_view xStr = line.substr(firstChar, secondChar - firstChar);
          std::string_view yStr = line.substr(secondChar);

          double x = ConvertToReal(xStr);
          double y = ConvertToReal(yStr);
          
          graph.Add2DCoordinate(x, y);
        }
      }

      if (graph.GetNumberOfVertices() <= 2000)
        graph.ObtainEdgesFrom2DPoints();

      return graph;
    }
  }
}