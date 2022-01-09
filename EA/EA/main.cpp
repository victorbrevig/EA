#include "pch.h"

#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "visualizer.h"
#include "tspPermutation.h"
#include <thread>
#include "blackBoxEA.h"
#include "lksearch.h"

void StartVisualizer(Visualizer* visualizer)
{
  if (visualizer != NULL)
    visualizer->StartVisualization();
}

int main()
{
  std::cout << "Hello World" << "\n";



  bool lkSearch = true;

  if (lkSearch)
  {
    Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\ch130.tsp");
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation);
    std::thread visualizerThread(StartVisualizer, visualizer);
    visualizerThread.detach();

    graph.UpdateNearNeighbors();

    std::cout << "Ready" << "\n";
    visualizer->WaitForSpace();

    LKSearch lkSearch(graph, visualizer);

    permutation.order = lkSearch.LinKernighan(permutation.order);
    permutation.updateFitness(graph);
    std::cout << "Parent 1 fitness: " << permutation.GetFitness() << "\n";
    visualizer->UpdatePermutation(permutation);
    visualizer->WaitForSpace();

    TSPpermutation permutation2((unsigned int)graph.GetNumberOfVertices());
    permutation2.order = lkSearch.LinKernighan(permutation2.order);
    permutation2.updateFitness(graph);
    std::cout << "Parent 2 fitness: " << permutation2.GetFitness() << "\n";
    visualizer->UpdatePermutation(permutation2);
    visualizer->WaitForSpace();

    visualizer->UpdatePermutation(std::vector<TSPpermutation>({ permutation, permutation2 }));
    visualizer->WaitForSpace();

    auto optionalChild = TSPpermutation::GPX(permutation, permutation2, graph);
    if (optionalChild.has_value())
    {
      TSPpermutation child = *optionalChild;
      child.updateFitness(graph);
      std::cout << "Child fitness: " << child.GetFitness() << "\n";
      visualizer->UpdatePermutation(child);
      visualizer->WaitForSpace();

      while (true)
      {
        std::cout << "Parent 1 fitness: " << permutation.GetFitness() << "\n";
        visualizer->UpdatePermutation(permutation);
        visualizer->WaitForSpace();

        std::cout << "Parent 2 fitness: " << permutation2.GetFitness() << "\n";
        visualizer->UpdatePermutation(permutation2);
        visualizer->WaitForSpace();

        visualizer->UpdatePermutation(std::vector<TSPpermutation>({ permutation, permutation2 }));
        visualizer->WaitForSpace();

        std::cout << "Child fitness: " << child.GetFitness() << "\n";
        visualizer->UpdatePermutation(child);
        visualizer->WaitForSpace();
      }
    }

    visualizer->WaitForClose();

    delete visualizer;
  }
  else
  {
    Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\bier127.tsp");
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation);
    std::thread visualizerThread(StartVisualizer, visualizer);
    visualizerThread.detach();

    BlackBoxEA<TSPpermutation>::Parameters parameters;
    parameters.iterations = (uint32_t)3e6;
    parameters.population = 50;
    parameters.mutationProb = 1.0;
    parameters.crossoverProb = 1.0 / parameters.population;
    BlackBoxEA<TSPpermutation>::Run(graph, parameters, visualizer);


    visualizer->WaitForClose();
    delete visualizer;
  }


  return 0;
}