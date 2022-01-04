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

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\usa13509.tsp");

  TSPpermutation permutation(graph.GetNumberOfVertices());

  //ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  
  Visualizer* visualizer = new Visualizer(graph, permutation);
  std::thread visualizerThread(StartVisualizer, visualizer);
  visualizerThread.detach();

  while(true)
  {
    bool res = permutation.mutate_2OPT(&graph);
    if (res)
      visualizer->UpdatePermutation(permutation);
  }


  visualizer->WaitForClose();
  delete visualizer;

  return 0;
}