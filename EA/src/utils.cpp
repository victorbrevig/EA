#include "pch.h"
#include "utils.h"


namespace Utils
{
  namespace Files
  {
    std::string StripDirFromFilePath(const std::string& filePath)
    {
      std::size_t found = filePath.find_last_of("/\\");
      return filePath.substr(0, found) + "\\";
    }

    std::string GetWorkingDirectory()
    {
      TCHAR buffer[MAX_PATH] = { 0 };
      GetModuleFileName(NULL, buffer, MAX_PATH);
      std::string exec = buffer;
      return StripDirFromFilePath(exec);
    }

    std::string ReadFile(const std::string& filePath)
    {
      std::ifstream t(filePath.c_str());
      if (t.fail())
      {
        //Try relative path
        std::string workingDir = GetWorkingDirectory();
        std::string newPath = workingDir + filePath;
        t.open(newPath);
        if (t.fail())
          return "";
      }

      t.seekg(0, std::ios::end);
      size_t size = (size_t)t.tellg();
      std::string buffer(size, ' ');
      t.seekg(0);
      t.read(&buffer[0], size);
      return buffer;
    }

  }

  namespace Parser
  {
    std::string GetNextLine(const std::string& fileContent, size_t& offset)
    {
      size_t newLine = fileContent.find('\n', offset);
      if (newLine == std::string::npos)
        return "";
      std::string ret = fileContent.substr(offset, newLine - offset);
      offset = newLine + 1;
      return ret;
    }
  }
}