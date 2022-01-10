#include "pch.h"

#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "visualizer.h"
#include "tspPermutation.h"
#include <thread>
#include "blackBoxEA.h"
#include "lksearch.h"
#include "bitstringProblems.h"

void StartVisualizer(Visualizer* visualizer)
{
  if (visualizer != NULL)
    visualizer->StartVisualization();
}

int main()
{
  std::cout << "Hello World" << "\n";

  enum class Job {
    TSP_BLACK_BOX,
    TSP_GRAY_BOX,
    TSP_BLACK_BOX_GENERATIONAL,
    SAT3__BLACK_BOX,
    SAT3__GRAY_BOX,
    SAT3__BLACK_BOX_GENERATIONAL
  };

  Job job = Job::SAT3__GRAY_BOX;

  switch (job)
  {
  case Job::TSP_BLACK_BOX:
    break;
  case Job::TSP_GRAY_BOX:
    break;
  case Job::TSP_BLACK_BOX_GENERATIONAL:
    break;
  case Job::SAT3__BLACK_BOX:
    break;
  case Job::SAT3__GRAY_BOX:
    BitstringProblems::RunGraybox("..\\ALL_3SAT\\UF250.1065.100\\uf250-03.cnf");
    break;
  case Job::SAT3__BLACK_BOX_GENERATIONAL:
    break;
  default:
    break;
  }

  return 0;

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