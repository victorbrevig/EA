#include "pch.h"
#include "utils.h"
#include <filesystem>


namespace Utils
{
  namespace Files
  {
    std::string GetNameFromFilePath(const std::string& filePath)
    {
      std::size_t found = filePath.find_last_of("/\\");
      return filePath.substr(found + 1);
    }

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

    std::vector<std::string> GetAllFilePathsInDirectory(const std::string& directory)
    {
      std::vector<std::string> ret;
      for (const auto& file : std::filesystem::directory_iterator(GetWorkingDirectory() + directory))
        ret.emplace_back(std::string(file.path().generic_string()));
      return ret;
    }

    void PrintLine(const std::string& str)
    {
      outputStream << str << "\n";
      std::cout << str << std::endl;
    }

    std::ofstream outputStream;
    void OpenOutputStream(const std::string& filePath)
    {
      outputStream.open(filePath);
    }
    void CloseOutputStream()
    {
      outputStream.close();
    }
  }

  namespace Parser
  {
    double ConvertToReal(const std::string_view& str)
    {
      size_t currentOffset = 0;

      //Stream functions
      auto HasNext = [&currentOffset, &str]() { return currentOffset < str.size(); };
      auto Peak = [&currentOffset, &str]() { return str[currentOffset]; };
      auto Consume = [&currentOffset]() { currentOffset++; };
      auto GetNext = [&currentOffset, &str]() { return str[currentOffset++]; };

      double signedMult = 1.0;
      if (HasNext() && Peak() == '-')
      {
        Consume();
        signedMult = -1;
      }

      auto GetDigit = [&GetNext]()
      {
        char digit = GetNext();
        int digitVal = digit - '0';
        return (double)digitVal;
      };

      //Get integer part
      double result = 0.0;
      while (HasNext() && isdigit(Peak()))
      {
        result *= 10;
        result += GetDigit();
      }

      if (HasNext() && Peak() == '.')
      {
        //Get decimal part
        Consume();
        double mul = 0.1f;
        while (HasNext() && isdigit(Peak()))
        {
          result += GetDigit() * mul;
          mul *= 0.1;
        }
      }

      double signedExponentMult = 1.0;
      double exponent = 0.0;

      if (HasNext() && Peak() == 'e')
      {
        Consume();
        if (HasNext() && (Peak() == '+' || Peak() == '-'))
        {
          signedExponentMult = (Peak() == '+') ? 1.0 : -1.0;
          Consume();
          exponent = 0.0;
          while (HasNext() && isdigit(Peak()))
          {
            exponent *= 10;
            exponent += GetDigit();
          }
        }
      }

      return signedMult * (result * pow(10, signedExponentMult * exponent));
    }

    std::string_view GetNextLine(const std::string_view& fileContent, size_t& offset)
    {
      size_t newLine = fileContent.find('\n', offset);
      if (newLine == std::string::npos)
      {
        offset = std::string::npos;
        return "";
      }

      std::string_view ret = fileContent.substr(offset, newLine - offset);
      offset = newLine + 1;
      return ret;
    }

    std::string GetNextLine(const std::string& fileContent, size_t& offset)
    {
      size_t newLine = fileContent.find('\n', offset);
      if (newLine == std::string::npos)
      {
        offset = std::string::npos;
        return "";
      }

      std::string ret = fileContent.substr(offset, newLine - offset);
      offset = newLine + 1;
      return ret;
    }
  }

  namespace OpenGL
  {

    GLuint CreateShaderProgramFromFiles(const std::string& vertexShaderfilePath, const std::string& fragmentShaderfilePath)
    {
      GLuint vertexShader = CreateShaderFromFile(vertexShaderfilePath, Utils::SHADER_TYPE::VERTEX);
      GLuint fragmentShader = CreateShaderFromFile(fragmentShaderfilePath, Utils::SHADER_TYPE::FRAGMENT);

      GLuint shaderProgram = glCreateProgram();
      glAttachShader(shaderProgram, vertexShader);
      glAttachShader(shaderProgram, fragmentShader);

      glLinkProgram(shaderProgram);

      glDeleteShader(vertexShader);
      glDeleteShader(fragmentShader);

      return shaderProgram;
    }

    GLuint CreateShaderFromFile(const std::string& filePath, SHADER_TYPE shaderType)
    {
      GLuint shader = glCreateShader(shaderType == SHADER_TYPE::VERTEX ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
      const std::string shaderSource = Files::ReadFile(filePath);
      const char* shaderSourcePtr = shaderSource.c_str();
      glShaderSource(shader, 1, &shaderSourcePtr, NULL);
      glCompileShader(shader);
      return shader;
    }
  }

  namespace Random
  {
    std::mt19937 engine{ std::random_device{}() };

    unsigned int Get()
    {
      return engine();
    }

    unsigned int GetRange(unsigned int min, unsigned int max)
    {
      return (Get() % (max - min + 1u)) + min;
    }

    std::pair<uint32_t, uint32_t> GetTwoDistinct(unsigned int min, unsigned int max)
    {
      ASSERT(max > min);
      uint32_t a = GetRange(min, max);
      uint32_t b = GetRange(a + 1, a + (max - min));
      if (b > max)
      {
        b -= (max - min);
        std::swap(a, b);
      } 
      
      return { a, b };
    }

    bool WithProbability(double p)
    {
      return (double)Get() / (double)engine.max() < p;
    }
  }
}