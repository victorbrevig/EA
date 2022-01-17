#include "pch.h"
#include "parser3SAT.h"
#include "utils.h"
#include "threeSATinstance.h"
namespace Utils
{
    namespace Parser
    {

        ThreeSATInstance parse3SAT(const std::string& filePath, bool equalVarsAndClauses) {
            std::string fileContent = Files::ReadFile(filePath);
            
            auto [vars, clauses] = getNumberOfVarsAndClauses(fileContent);
            if (equalVarsAndClauses)
              clauses = vars;
            ThreeSATInstance threeSATinstance(
                clauses,
                vars, 
                getVector3SAT(fileContent, clauses)
            );
            return threeSATinstance;
        }



        std::vector<int> getVector3SAT(const std::string& fileContent, int numberOfClauses) {
            
            size_t offset = 0;
            // first info lines
            std::string line;
            do
            {
              line = GetNextLine(fileContent, offset);
            } while (line[0] != 'p');


            std::string space_delimiter = " ";
            size_t pos = 0;
            
            // Contains information about all clauses. Shift by 3.
            std::vector<int> res(numberOfClauses * 3);
            line = GetNextLine(fileContent, offset);
            // first clause might contain a white space as first char
            while(line[0] == ' ')
              line = line.substr(1, line.size() - 1);

            // Iterate thorugh clauses
            int count = 0;
            for (int i = 0; i < numberOfClauses; i++) {
                
                pos = 0;

                while ((pos = line.find(space_delimiter)) != std::string::npos) {
                  if (line[0] != ' ')
                    res[count++] = std::stoi(line.substr(0, pos));
                  line.erase(0, pos + space_delimiter.length());
                }
                line = GetNextLine(fileContent, offset);
                
            }
            return res;            
        }

        std::pair<int, int> getNumberOfVarsAndClauses(const std::string& fileContent) {
            size_t offset = 0;
            // first info lines
            std::string line;
            do
            {
              line = GetNextLine(fileContent, offset);
            } while (line[0] != 'p');

            std::vector<std::string> words{};
            char space_delimiter = ' ';
            size_t pos = 0;
            while ((pos = line.find(space_delimiter)) != std::string::npos) {
              if (line[0] != ' ')
                words.push_back(line.substr(0, pos));
              line.erase(0, pos + 1);
            }

            words.push_back(line);
            
            return { std::stoi(words[2]), std::stoi(words[3])};
        }
    }
} 

