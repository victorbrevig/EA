#include "pch.h"
#include "tspPermutation.h"
#include <array>
#include "Headers\blackBoxEA.h"
#include "permutationProblems.h"

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
  auto parents = Utils::Random::GetTwoDistinct(0, (unsigned int)(population.size() - 1));
  TSPpermutation& p1 = population[parents.first];
  TSPpermutation& p2 = population[parents.second];


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
    if (currentNumberOfIterations > maxNumberOfIterations) {
        return false;
    }
        
    currentNumberOfIterations++;
    
    
    std::vector<TSPpermutation> newPopulation;
    newPopulation.reserve(population.size());
    while (newPopulation.size() < population.size())
    {
        
        auto [index1, index2] = selection();
        TSPpermutation& p1 = population[index1];
        TSPpermutation& p2 = population[index2];

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
            TSPpermutation child1 = population[index1];
            if (Utils::Random::WithProbability(mutationProb)) child1.mutate_2OPT(graph);
            newPopulation.emplace_back(std::move(child1));
            if (newPopulation.size() < population.size()) {
                TSPpermutation child2 = population[index2];
                if (Utils::Random::WithProbability(mutationProb)) child2.mutate_2OPT(graph);
                newPopulation.emplace_back(std::move(child2));
            }
        }  
    }
    std::swap(newPopulation, population);
    return true;
}


PermutationProblems::Result BlackBoxEA<TSPpermutation>::Run(const Graph& graph, Parameters parameters, bool generational, Visualizer* visualizer)
{
  std::vector<TSPpermutation> population;
  population.reserve(parameters.population);
  for (size_t i = 0; i < parameters.population; i++)
    population.emplace_back((unsigned int)graph.GetNumberOfVertices());

  BlackBoxEA<TSPpermutation> ea(population, (unsigned int)parameters.iterations, parameters.mutationProb, parameters.crossoverProb);


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
      visualizer->UpdatePermutation(population[0].order);
  }

  if (visualizer)
    visualizer->UpdatePermutation(population[0].order, true);

  for (TSPpermutation& individual : population)
    individual.updateFitness(graph);

  struct {
    bool operator()(TSPpermutation& a, TSPpermutation& b) const { return a.GetFitness() < b.GetFitness(); }
  } customLess;

  //auto bestFitness = std::min_element(population.begin(), population.end(), customLess);
  std::sort(population.begin(), population.end(), customLess);


  return { (uint32_t)population[0].GetFitness(), parameters.iterations };

}


template<class T>
std::pair<uint32_t, uint32_t> BlackBoxEA<T>::selection()
{
    auto GetParent = [this]() {
        auto [p1, p2] = Utils::Random::GetTwoDistinct(0, (uint32_t)population.size() - 1);
        bool b = Utils::Random::WithProbability(0.5);
        uint32_t p1_unordered = b ? p1 : p2;
        uint32_t p2_unordered = b ? p2 : p1;

        if (population[p1_unordered].GetFitness() > population[p2_unordered].GetFitness())
            return p1_unordered;
        return p2_unordered;
    };


    return { GetParent(), GetParent() };
}