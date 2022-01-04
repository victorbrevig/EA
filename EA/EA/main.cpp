#include "pch.h"

#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "visualizer.h"
#include "tspPermutation.h"
#include <thread>

void StartVisualizer(Visualizer* visualizer)
{
  if (visualizer != NULL)
    visualizer->StartVisualization();
}

int main()
{
  std::cout << "Hello World" << "\n";

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\bier127.tsp");

  TSPpermutation permutation(graph);

  //ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  
  Visualizer* visualizer = new Visualizer(graph, permutation);
  std::thread visualizerThread(StartVisualizer, visualizer);
  visualizerThread.detach();

  visualizer->WaitForSpace();
  TSPpermutation permutation1(graph);
  visualizer->UpdatePermutation(permutation1);

  visualizer->WaitForSpace();
  TSPpermutation permutation2(graph);
  visualizer->UpdatePermutation(permutation2);

  visualizer->WaitForClose();
  delete visualizer;

  return 0;
}