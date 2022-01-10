#include "pch.h"
#include "bitstringProblems.h"
#include "parser3SAT.h"
#include "greyBoxEA.h"

namespace BitstringProblems
{

  void RunGraybox(const std::string& file)
  {
    ThreeSATInstance instance = Utils::Parser::parse3SAT(file);

    uint32_t iterations = 10000;
    uint32_t population = 5; //50
    double crossoverProb = 1.0; //0.6

    Hybrid hybrid(population, crossoverProb, instance);

    for (uint32_t i = 0; i < iterations; i++)
    {
      hybrid.Iterate(i);
      hybrid.PrintBestSolution();
    }
  }

  Hybrid::Hybrid(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance)
    : population(populationSize, _threeSATInstance.numberOfVariables), crossoverProb(_crossoverProb), threeSATInstance(_threeSATInstance)
  {
    for (Bitstring& bitstring : population)
    {
      bitstring.MutationLSFI(threeSATInstance);
    }
  }

  void Hybrid::Iterate(uint32_t iteration)
  {
    uint32_t bestSolutionIndex = GetBestSolution();

    std::vector<Bitstring> newPopulation;
    newPopulation.reserve(population.size());
    newPopulation.emplace_back(population[bestSolutionIndex]);
    while (newPopulation.size() < population.size())
    {
      auto [p1, p2] = Selection();

      

      if (Utils::Random::WithProbability(crossoverProb))
      {
        //Crossover
        Bitstring::GPX(population[p1], population[p2], threeSATInstance);
      }
      else
      {
        //Mutation
        Bitstring parent1Bitstring = population[p1];
        parent1Bitstring.Mutation(3.0 / (double)parent1Bitstring.content.size());
        newPopulation.emplace_back(std::move(parent1Bitstring));
      }

      
    }

    if (iteration % 10 == 0)
    {
      uint32_t bestSolutionIndex = GetBestSolution();
      newPopulation[0] = newPopulation[bestSolutionIndex];
      for (uint32_t i = 0; i < population.size() / 10; i++)
      {
        newPopulation[i].MutationLSFI(threeSATInstance);
      }
      for (uint32_t i = (uint32_t)population.size() / 10 + 1; i < population.size(); i++)
      {
        newPopulation[i] = Bitstring((uint32_t)newPopulation[i].content.size());
        newPopulation[i].MutationLSFI(threeSATInstance);
      }
    }

    std::swap(newPopulation, population);
  }

  std::pair<uint32_t, uint32_t> Hybrid::Selection()
  {
    std::array<uint32_t, 5> tournamentIndex;
    for (uint32_t i = 0; i < tournamentIndex.size(); i++)
    {
      tournamentIndex[i] = Utils::Random::GetRange(0, (uint32_t)population.size() - 1);
    }

    uint32_t bestBitstringIndex = tournamentIndex[0];
    uint32_t bestFitness = population[bestBitstringIndex].GetFitness(threeSATInstance);
    for (uint32_t i = 1; i < tournamentIndex.size(); i++)
    {
      uint32_t index = tournamentIndex[i];
      const Bitstring& bitstring = population[index];
      uint32_t fitness = bitstring.GetFitness(threeSATInstance);
      if (fitness > bestFitness)
      {
        bestFitness = fitness;
        bestBitstringIndex = index;
      }
    }

    uint32_t parent1 = bestBitstringIndex;
    const Bitstring& parent1Bitstring = population[parent1];
    uint32_t parent2 = parent1;
    uint32_t bestHammingDistance = INT32_MAX;
    for (uint32_t i = 0; i < tournamentIndex.size(); i++)
    {
      uint32_t index = tournamentIndex[i];
      if (index != parent1)
      {
        uint32_t hammingDistance = Bitstring::HammingDistance(parent1Bitstring, population[index]);
        if (hammingDistance < bestHammingDistance)
        {
          bestHammingDistance = hammingDistance;
          parent2 = index;
        }
      }
    }

    return {parent1, parent2};
  }

  uint32_t Hybrid::GetBestSolution()
  {
    uint32_t bestBitstringIndex = 0;
    uint32_t bestFitness = population[0].GetFitness(threeSATInstance);
    for (uint32_t i = 1; i < population.size(); i++)
    {
      const Bitstring& bitstring = population[i];
      uint32_t fitness = bitstring.GetFitness(threeSATInstance);
      if (fitness > bestFitness)
      {
        bestBitstringIndex = i;
        bestFitness = fitness;
      }
    }

    return bestBitstringIndex;
  }

  void Hybrid::PrintBestSolution()
  {
    uint32_t bestSolutionIndex = GetBestSolution();
    uint32_t fitness = population[bestSolutionIndex].GetFitness(threeSATInstance);
    std::cout << "Best fitness: " << fitness << "\n";
  }
}