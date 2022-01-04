#include "pch.h"

#include "Headers\blackBoxTSP.h"

BlackBoxTSP::BlackBoxTSP(unsigned int _populationSize, unsigned int _maxNumberOfIterations, float _mutationProb, float _crossoverProb)
  : population(_populationSize)
{
  maxNumberOfIterations = _maxNumberOfIterations;
  mutationProb = _mutationProb;
  crossoverProb = _crossoverProb;
}

void BlackBoxTSP::iterate(const Graph& graph)
{
  if (currentNumberOfIterations > maxNumberOfIterations) {return;}

  for (unsigned int i = 0; i < population.size(); i++) {
    // SELECTION - FIND TWO PARENTS
    unsigned int rand1 = 0;
    unsigned int rand2 = 1;
    TSPpermutation p1 = population[rand1];
    TSPpermutation p2 = population[rand2];

    // CROSSOVER
    TSPpermutation child = TSPpermutation::orderCrossover(p1,p2);

    
    // MUTATE CHILD

    // ADD NEW CHILD TO POPULAITON IF BETTER
    


  }
}
