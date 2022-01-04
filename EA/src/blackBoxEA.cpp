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
  uint32_t rand1 = Utils::Random::GetRange(0, population.size() - 2);
  uint32_t rand2 = Utils::Random::GetRange(rand1 + 1, population.size() - 1);
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
