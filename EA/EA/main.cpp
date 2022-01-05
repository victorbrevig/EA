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

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\bier127.tsp");

  TSPpermutation permutation(graph.GetNumberOfVertices());

  //ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  
  Visualizer* visualizer = new Visualizer(graph, permutation);
  std::thread visualizerThread(StartVisualizer, visualizer);
  visualizerThread.detach();

  while(true)
  {
    TSPpermutation permutation(graph.GetNumberOfVertices());
    LKSearch lkSearch(graph, visualizer);

    permutation.order = lkSearch.LinKernighan(permutation.order);
    permutation.order = lkSearch.LinKernighan(permutation.order);
    permutation.order = lkSearch.LinKernighan(permutation.order);
    permutation.order = lkSearch.LinKernighan(permutation.order);
    permutation.updateFitness(graph);
    std::cout << ": Fitness: " << permutation.GetFitness() << "\n";
    visualizer->UpdatePermutation(permutation);
    visualizer->WaitForSpace();
  }

  if (false)
  {

    std::vector<TSPpermutation> population;
    population.reserve(2);
    for (size_t i = 0; i < 2; i++)
      population.emplace_back(graph.GetNumberOfVertices());

    BlackBoxEA<TSPpermutation> ea(population, 1e8, 1.0, 0);

    bool isDone = false;

    while (!isDone)
    {
      for (size_t i = 0; i < 1000; i++)
      {
        if (!ea.iterate(graph))
        {
          isDone = true;
          break;
        }
      }


      visualizer->UpdatePermutation(population);
    }

    for (TSPpermutation& individual : population)
      individual.updateFitness(graph);

    struct {
      bool operator()(TSPpermutation& a, TSPpermutation& b) const { return a.GetFitness() < b.GetFitness(); }
    } customLess;

    //auto bestFitness = std::min_element(population.begin(), population.end(), customLess);
    std::sort(population.begin(), population.end(), customLess);

    uint32_t individualIndex = 0;
    while(true)
    {
      std::cout << individualIndex << ": Fitness: " << population[individualIndex].GetFitness() << "\n";
      visualizer->UpdatePermutation(population[individualIndex]);
      individualIndex = (individualIndex + 1) % population.size();
      if (individualIndex == 0)
        break;
    }

  }


  visualizer->WaitForClose();
  delete visualizer;

  return 0;
}