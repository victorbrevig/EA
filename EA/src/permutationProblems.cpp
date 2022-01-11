#include "pch.h"
#include "permutationProblems.h"
#include "graph.h"
#include "visualizer.h"
#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "lksearch.h"
#include "blackBoxEA.h"
#include <thread>

namespace PermutationProblems 
{

  void StartVisualizer(Visualizer* visualizer)
  {
    if (visualizer != NULL)
      visualizer->StartVisualization();
  }

  void RunGraybox(const std::string& file)
  {
    Graph graph = Utils::Parser::ParseTSPGraph(file);
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation.order);
    std::thread visualizerThread(StartVisualizer, visualizer);
    visualizerThread.detach();

    graph.UpdateNearNeighbors();

    std::cout << "Ready" << "\n";
    visualizer->WaitForSpace();


    while (true)
    {
      TSPpermutation parent1((unsigned int)graph.GetNumberOfVertices());
      parent1.LinKernighan(graph, visualizer);
      std::cout << "Parent 1 fitness: " << parent1.GetFitness() << "\n";
      visualizer->UpdatePermutation(parent1.order);
      visualizer->WaitForSpace();

      TSPpermutation parent2((unsigned int)graph.GetNumberOfVertices());
      parent2.LinKernighan(graph, visualizer);
      std::cout << "Parent 2 fitness: " << parent2.GetFitness() << "\n";
      visualizer->UpdatePermutation(parent2.order);
      visualizer->WaitForSpace();

      visualizer->UpdatePermutation(std::vector<std::vector<uint32_t>>({ parent1.order, parent2.order }));
      visualizer->WaitForSpace();


      auto optionalChild = TSPpermutation::GPX(parent1, parent2, graph);
      if (optionalChild.has_value())
      {
        std::pair<TSPpermutation, TSPpermutation> children = *optionalChild;
        TSPpermutation greedyChild = children.first;
        TSPpermutation otherChild = children.second;
        greedyChild.updateFitness(graph);
        std::cout << "Child fitness: " << greedyChild.GetFitness() << "\n";
        visualizer->UpdatePermutation(greedyChild.order);
        visualizer->WaitForSpace();



        visualizer->UpdatePermutation(otherChild.order);
        visualizer->WaitForSpace();




        std::cout << "Parent 1 fitness: " << parent1.GetFitness() << "\n";
        visualizer->UpdatePermutation(parent1.order);
        visualizer->WaitForSpace();

        std::cout << "Parent 2 fitness: " << parent2.GetFitness() << "\n";
        visualizer->UpdatePermutation(parent2.order);
        visualizer->WaitForSpace();

        visualizer->UpdatePermutation(std::vector<std::vector<uint32_t>>({ parent1.order, parent2.order }));
        visualizer->WaitForSpace();

        std::cout << "Child fitness: " << greedyChild.GetFitness() << "\n";
        visualizer->UpdatePermutation(greedyChild.order);
        visualizer->WaitForSpace();
      }
    }

    visualizer->WaitForClose();

    delete visualizer;
  }

  void RunBlackbox1(const std::string& file)
  {
    Graph graph = Utils::Parser::ParseTSPGraph(file);
    TSPpermutation permutation((unsigned int)graph.GetNumberOfVertices());
    Visualizer* visualizer = new Visualizer(graph, permutation.order);
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
};
