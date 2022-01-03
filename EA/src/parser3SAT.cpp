#include "pch.h"
#include "parser3SAT.h"
#include "utils.h"
namespace Utils
{
    namespace Parser
    {

        std::vector<int> getVector3SAT(const std::string& filePath) {
            std::string fileContent = Files::ReadFile(filePath);

            size_t offset = 0;
            // first info lines
            for (int i = 0; i < 7; i++) {
                GetNextLine(fileContent, offset);
            }
            std::string line = GetNextLine(fileContent, offset);
            line = GetNextLine(fileContent, offset);


            
            std::vector<std::string> words{};
            std::string space_delimiter = " ";
            size_t pos = 0;
            while ((pos = line.find(space_delimiter)) != std::string::npos) {
                words.push_back(line.substr(0, pos));
                line.erase(0, pos + space_delimiter.length());
            }
            const int numberOfVars = std::stoi(words[2]);
            const int numberOfClauses = std::stoi(words[3]);


            // Contains information about all clauses. Shift by 3.
            std::vector<int> res(numberOfClauses*3);

            // Iterate thorugh clauses
            int count = 0;
            for (int i = 0; i < numberOfClauses; i++) {
                line = GetNextLine(fileContent, offset);
                pos = 0;

                while ((pos = line.find(space_delimiter)) != std::string::npos) {
                    res[count] = std::stoi(line.substr(0, pos));
                    line.erase(0, pos + space_delimiter.length());
                    count++;
                }
                
            }

            return res;            

            





        }




    }
} 

