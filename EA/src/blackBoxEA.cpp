#include "pch.h"
#include "tspPermutation.h"
#include <array>
#include "Headers\blackBoxEA.h"

BlackBoxEA<TSPpermutation>::BlackBoxEA(std::vector<TSPpermutation>& population, unsigned int _maxNumberOfIterations, double _mutationProb, double _crossoverProb)
  : population(population)
{
  maxNumberOfIterations = _maxNumberOfIterations;
  mutationProb = _mutationProb;
  crossoverProb = _crossoverProb;
}

bool BlackBoxEA<TSPpermutation>::iterate(const Graph& graph)
{
  if (currentNumberOfIterations > maxNumberOfIterations)
    return false;
  currentNumberOfIterations++;

  // SELECTION - FIND TWO PARENTS
  uint32_t rand1 = Utils::Random::GetRange(0, (unsigned int)population.size() - 2);
  uint32_t rand2 = Utils::Random::GetRange(rand1 + 1, (unsigned int)population.size() - 1);
  TSPpermutation& p1 = population[rand1];
  TSPpermutation& p2 = population[rand2];


  // CROSSOVER
  if (Utils::Random::WithProbability(crossoverProb))
  {
    p1.updateFitness(graph);
    p2.updateFitness(graph);
    TSPpermutation child1 = TSPpermutation::orderCrossover(p1, p2);
    TSPpermutation child2 = TSPpermutation::orderCrossover(p2, p1);
    child1.updateFitness(graph);
    child2.updateFitness(graph);

    std::array<TSPpermutation*, 4> pop = { &p1, &p2, &child1, &child2 };

    struct {
      bool operator()(TSPpermutation* a, TSPpermutation* b) const { return a->GetFitness() < b->GetFitness(); }
    } customLess;

    std::sort(pop.begin(), pop.end(), customLess);

    p1 = *pop[0];
    p2 = *pop[1];
  }

  // MUTATE CHILD to become better
  if (Utils::Random::WithProbability(mutationProb))
  {
    p1.mutate_2OPT(graph);
    p2.mutate_2OPT(graph);
  }

  return true;
}

template<class T>
bool BlackBoxEA<T>::iterateGenerational(const Graph& graph)
{
    if (currentNumberOfIterations > maxNumberOfIterations)
        return false;
    currentNumberOfIterations++;
    
    
    std::vector<TSPpermutation> newPopulation;
    newPopulation.reserve(population.size());
    while (newPopulation.size() < population.size())
    {
        // randomly select two parents
        uint32_t rand1 = Utils::Random::GetRange(0, (unsigned int)population.size() - 2);
        uint32_t rand2 = Utils::Random::GetRange(rand1 + 1, (unsigned int)population.size() - 1);
        TSPpermutation& p1 = population[rand1];
        TSPpermutation& p2 = population[rand2];

        if (Utils::Random::WithProbability(crossoverProb)) {
            p1.updateFitness(graph);
            p2.updateFitness(graph);
            TSPpermutation child1 = TSPpermutation::orderCrossover(p1, p2);
            TSPpermutation child2 = TSPpermutation::orderCrossover(p2, p1);
            child1.updateFitness(graph);
            child2.updateFitness(graph);

            if (Utils::Random::WithProbability(mutationProb)) child1.mutate_2OPT(graph);
            newPopulation.emplace_back(std::move(child1));
            
            if (newPopulation.size() < population.size())
            {
                if (Utils::Random::WithProbability(mutationProb)) child2.mutate_2OPT(graph);
                newPopulation.emplace_back(std::move(child2));
            }
        }
        else {
            TSPpermutation child1 = population[rand1];
            if (Utils::Random::WithProbability(mutationProb)) child1.mutate_2OPT(graph);
            newPopulation.emplace_back(std::move(child1));
            if (newPopulation.size() < population.size()) {
                TSPpermutation child2 = population[rand2];
                if (Utils::Random::WithProbability(mutationProb)) child2.mutate_2OPT(graph);
                newPopulation.emplace_back(std::move(child2));
            }
        }  
    }
    std::swap(newPopulation, population);
    return true;
}


void BlackBoxEA<TSPpermutation>::Run(const Graph& graph, Parameters parameters, bool generational, Visualizer* visualizer)
{
  std::vector<TSPpermutation> population;
  population.reserve(parameters.population);
  for (size_t i = 0; i < parameters.population; i++)
    population.emplace_back((unsigned int)graph.GetNumberOfVertices());

  BlackBoxEA<TSPpermutation> ea(population, (unsigned int)parameters.iterations, 1.0, 0);


  while (true)
  {
    if (!generational) {
        if (!ea.iterate(graph))
            break;
    }
    else {
        if (!ea.iterateGenerational(graph))
            break;
    }

    if (visualizer)
      visualizer->UpdatePermutation(population[0].order, true);
  }

  if (visualizer)
    visualizer->UpdatePermutation(population[0].order);

  for (TSPpermutation& individual : population)
    individual.updateFitness(graph);

  struct {
    bool operator()(TSPpermutation& a, TSPpermutation& b) const { return a.GetFitness() < b.GetFitness(); }
  } customLess;

  //auto bestFitness = std::min_element(population.begin(), population.end(), customLess);
  std::sort(population.begin(), population.end(), customLess);

  uint32_t individualIndex = 0;
  while (true)
  {
    std::cout << individualIndex << ": Fitness: " << population[individualIndex].GetFitness() << "\n";
    if (visualizer)
    {
      visualizer->UpdatePermutation(population[individualIndex].order);
      visualizer->WaitForSpace();
    }

    individualIndex = (individualIndex + 1) % population.size();
    if (individualIndex == 0)
      break;
  }
}