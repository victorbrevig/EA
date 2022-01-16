#include <string>
#include <threeSATinstance.h>
namespace Utils
{
  namespace Parser
  {
      ThreeSATInstance parse3SAT(const std::string& filePath, bool equalVarsAndClauses = false);
      std::vector<int> getVector3SAT(const std::string& fileContent, int numberOfClauses);
      std::pair<int, int> getNumberOfVarsAndClauses(const std::string& fileContent);
  }
}