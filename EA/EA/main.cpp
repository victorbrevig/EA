#include "pch.h"

#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"

int main()
{
  std::cout << "Hello World" << "\n";

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\d198.tsp");

  ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  

  return 0;
}