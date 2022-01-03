#include "pch.h"

#include "utils.h"
#include "parserTSP.h"

int main()
{
  std::cout << "Hello World" << "\n";

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\d198.tsp");


  

  return 0;
}