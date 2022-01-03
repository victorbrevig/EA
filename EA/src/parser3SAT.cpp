#include "pch.h"
#include "parser3SAT.h"
#include "utils.h"
#include "threeSATinstance.h"
namespace Utils
{
    namespace Parser
    {

        ThreeSATInstance parse3SAT(const std::string& filePath) {
            std::string fileContent = Files::ReadFile(filePath);
            
            auto [vars, clauses] = getNumberOfVarsAndClauses(fileContent);
            
            ThreeSATInstance threeSATinstance(
                vars, 
                clauses, 
                getVector3SAT(fileContent, clauses)
            );
            return threeSATinstance;
        }



        std::vector<int> getVector3SAT(const std::string& fileContent, int numberOfClauses) {
            
            size_t offset = 0;
            // first info lines
            for (int i = 0; i < 7; i++) {
                GetNextLine(fileContent, offset);
            }
            std::string line = GetNextLine(fileContent, offset);
            
            std::string space_delimiter = " ";
            size_t pos = 0;
            
            // Contains information about all clauses. Shift by 3.
            std::vector<int> res(numberOfClauses*3);
            line = GetNextLine(fileContent, offset);
            // first clause contains a white space as first char
            line = line.substr(1, line.size() - 1);
            // Iterate thorugh clauses
            int count = 0;
            for (int i = 0; i < numberOfClauses; i++) {
                
                pos = 0;

                while ((pos = line.find(space_delimiter)) != std::string::npos) {
                    res[count] = std::stoi(line.substr(0, pos));
                    line.erase(0, pos + space_delimiter.length());
                    count++;
                }
                line = GetNextLine(fileContent, offset);
                
            }
            return res;            
        }

        std::pair<int, int> getNumberOfVarsAndClauses(const std::string& fileContent) {
            size_t offset = 0;
            // first info lines
            for (int i = 0; i < 7; i++) {
                GetNextLine(fileContent, offset);
            }
            std::string line = GetNextLine(fileContent, offset);

            std::vector<std::string> words{};
            char space_delimiter = ' ';
            size_t pos = 0;
            while ((pos = line.find(space_delimiter)) != std::string::npos) {
                words.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            
            return { std::stoi(words[2]), std::stoi(words[4]) };
        }
    }
} 

