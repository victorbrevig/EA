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

  TSPpermutation permutation(graph.GetNumberOfVertices());

  //ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  
  Visualizer* visualizer = new Visualizer(graph, permutation);
  std::thread visualizerThread(StartVisualizer, visualizer);
  visualizerThread.detach();

  visualizer->WaitForSpace();
  TSPpermutation permutation1(graph.GetNumberOfVertices());
  visualizer->UpdatePermutation(permutation1);

  visualizer->WaitForSpace();
  TSPpermutation permutation2(graph.GetNumberOfVertices());
  visualizer->UpdatePermutation(permutation2);

  // ORDER CROSSOVER
  visualizer->WaitForSpace();
  TSPpermutation oxChild = TSPpermutation::orderCrossover(permutation1, permutation2);
  visualizer->UpdatePermutation(oxChild);

  visualizer->WaitForClose();
  delete visualizer;

  return 0;
}