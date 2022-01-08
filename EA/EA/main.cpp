#include "pch.h"

#include "utils.h"
#include "parserTSP.h"
#include "parser3SAT.h"
#include "visualizer.h"
#include "tspPermutation.h"
#include <thread>
#include "blackBoxEA.h"

void StartVisualizer(Visualizer* visualizer)
{
  if (visualizer != NULL)
    visualizer->StartVisualization();
}

int main()
{
  std::cout << "Hello World" << "\n";

  Graph graph = Utils::Parser::ParseTSPGraph("..\\ALL_TSP\\burma14.tsp");

  TSPpermutation permutation(graph.GetNumberOfVertices());

  /*
  std::vector<uint32_t> order1 = { 4,13,5,9,0,11,1,12,6,7,2,3,10,8 };
  TSPpermutation p1(order1);
  std::vector<uint32_t> order2 = { 9,2,3,4,0,1,12,13,5,11,10,6,7,8 };
  TSPpermutation p2(order2);
  TSPpermutation p3 = TSPpermutation::GPX(p1, p2, graph);
  */


  //ThreeSATInstance threeSATinstance = Utils::Parser::parse3SAT("..\\ALL_3SAT\\uf20-01.cnf");
  
  Visualizer* visualizer = new Visualizer(graph, permutation);
  std::thread visualizerThread(StartVisualizer, visualizer);
  visualizerThread.detach();

  while(false)
  {
    bool res = permutation.mutate_2OPT(graph);
    if (res)
      visualizer->UpdatePermutation(permutation);
  }
  
  std::vector<TSPpermutation> population;
  population.reserve(2);
  for (size_t i = 0; i < 2; i++)
    population.emplace_back(graph.GetNumberOfVertices());

  BlackBoxEA<TSPpermutation> ea(population, 1e5, 1.0, 0);

  bool isDone = true;

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


    //visualizer->UpdatePermutation(population);
  }

  for (TSPpermutation& individual : population)
    individual.updateFitness(graph);

  struct {
    bool operator()(TSPpermutation a, TSPpermutation b) const { return a.GetFitness() < b.GetFitness(); }
  } customLess;

  //auto bestFitness = std::min_element(population.begin(), population.end(), customLess);
  std::sort(population.begin(), population.end(), customLess);

  

  //TSPpermutation p1 = population[0];
  //TSPpermutation p2 = population[100];
  std::vector<uint32_t> order1 = { 4,13,5,9,0,11,1,12,6,7,2,3,10,8 };
  TSPpermutation p1(order1);
  std::vector<uint32_t> order2 = { 9,2,3,4,0,1,12,13,5,11,10,6,7,8 };
  TSPpermutation p2(order2);
  TSPpermutation p3 = TSPpermutation::GPX(p1, p2, graph);

  std::vector<TSPpermutation> parentPerms = { p1,p2 };

  
  while (true) {
      visualizer->UpdatePermutation(parentPerms);
      visualizer->WaitForSpace();
      visualizer->UpdatePermutation(p3);
      visualizer->WaitForSpace();
  }
  
  

  /*
  uint32_t individualIndex = 0;
  while(true)
  {
    std::cout << individualIndex << ": Fitness: " << population[individualIndex].GetFitness() << "\n";
    visualizer->UpdatePermutation(population[individualIndex]);
    individualIndex = (individualIndex + 1) % population.size();
    if (individualIndex == 0)
      break;
  }
  */
  
  
  



  visualizer->WaitForClose();
  delete visualizer;



  return 0;
}