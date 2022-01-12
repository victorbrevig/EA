#include "pch.h"
#include "bitstringProblems.h"
#include "parser3SAT.h"
#include "greyBoxEA.h"
#include <ctime>
#include <chrono>

namespace BitstringProblems
{
  bool ShouldStop(long long msStart, uint32_t milliseconds)
  {
    long long timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return (uint32_t)(timeNow - msStart) >= milliseconds;
  }

  Result RunHybrid(const std::string& file, HybridVersion hybridVersion, uint32_t runningTimeMilliseconds)
  {
    auto HybridVersionToString = [](HybridVersion hybridVersion) {
      switch (hybridVersion)
      {
      case BitstringProblems::HybridVersion::PartionCrossover:
        return "Partition Crossover";
      case BitstringProblems::HybridVersion::TwoPointCrossover:
        return "Two point Crossover";
      case BitstringProblems::HybridVersion::TwoPointCrossoverImproved:
        return "Two point Crossover improved by local search";
      case BitstringProblems::HybridVersion::NoCrossover:
        return "No Crossover";
      default:
        break;
      }

      return "";
    };

    ThreeSATInstance instance = Utils::Parser::parse3SAT(file);
    std::cout << "--------------------------------------------- \n";
    std::cout << "Job start \n";
    std::cout << "Search space: Bitstring \n";
    std::cout << "Problem: Max 3-SAT \n";
    std::cout << "Problem instance: " << file << "\n";
    std::cout << "Algorithm: " << "Hybrid with " << HybridVersionToString(hybridVersion) << "\n";
    std::cout << "Running Time: " << runningTimeMilliseconds << " milliseconds\n";

    uint32_t population = 50; //50
    double crossoverProb = hybridVersion == HybridVersion::NoCrossover ? 0.0 : 0.6; //0.6

    Hybrid hybrid(population, crossoverProb, instance, hybridVersion);

    long long timeStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    uint32_t bestFitnessSoFar = 0;
    uint32_t iterations = 0;
    while (!ShouldStop(timeStart, runningTimeMilliseconds))
    {
      hybrid.Iterate(iterations);
      uint32_t fitness = hybrid.GetBestFitness();
      if (fitness > bestFitnessSoFar)
      {
        std::cout << "Iteration: " << iterations << ", Fitness: " << fitness << "\n";
        bestFitnessSoFar = fitness;
      }
      iterations++;
    }
    std::cout << "------------------------------- \n";
    std::cout << "Job Complete \n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Best Solution Fitness: " << bestFitnessSoFar << " \n";
    if (bestFitnessSoFar == instance.numberOfClauses)
    {
      std::cout << "Optimal \n";
    }
    else
    {
      std::cout << "Not Optimal \n";
    }

    if (hybridVersion == HybridVersion::TwoPointCrossoverImproved)
    {
      std::cout << "------------------------------- \n";
      std::cout << "Local optimality of 2-point crossover children\n";
      std::cout << "Local optimal count: " << hybrid.timesTwoPointCrossoverWasLocalOptimum << "\n";
      std::cout << "Not local optimal count: " << hybrid.timesTwoPointCrossoverWasNotLocalOptimum << "\n";
    }

    std::cout << "--------------------------------------------- " << std::endl;
    return { bestFitnessSoFar, iterations, bestFitnessSoFar == instance.numberOfClauses };
  }

  Result RunBlackBoxGenerational(const std::string& file, uint32_t runningTimeMilliseconds)
  {
    ThreeSATInstance instance = Utils::Parser::parse3SAT(file);

    std::cout << "--------------------------------------------- \n";
    std::cout << "Job start \n";
    std::cout << "Search space: Bitstring \n";
    std::cout << "Problem: Max 3-SAT \n";
    std::cout << "Problem instance: " << file << "\n";
    std::cout << "Algorithm: Black-Box Generational \n";
    std::cout << "Running Time: " << runningTimeMilliseconds << " milliseconds\n";

    uint32_t population = 500;
    double crossoverProb = 0.7;

    BlackBoxGenerational blackBoxGenerational(population, crossoverProb, instance);

    long long timeStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    uint32_t bestFitnessSoFar = 0;
    uint32_t iterations = 0;
    while (!ShouldStop(timeStart, runningTimeMilliseconds))
    {
      blackBoxGenerational.Iterate(iterations);
      uint32_t fitness = blackBoxGenerational.GetBestFitness();
      if (fitness > bestFitnessSoFar)
      {
        std::cout << "Iteration: " << iterations << ", Fitness: " << fitness << "\n";
        bestFitnessSoFar = fitness;
      }
    }
    std::cout << "------------------------------- \n";
    std::cout << "Job Complete \n";
    std::cout << "Iterations: " << iterations << "\n";
    std::cout << "Best Solution Fitness: " << bestFitnessSoFar << " \n";
    if (bestFitnessSoFar == instance.numberOfClauses)
    {
      std::cout << "Optimal \n";
    }
    else
    {
      std::cout << "Not Optimal \n";
    }
    std::cout << "--------------------------------------------- " << std::endl;

    return { bestFitnessSoFar, iterations, bestFitnessSoFar == instance.numberOfClauses };
  }

  Hybrid::Hybrid(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance, HybridVersion _hybridVersion)
    : population(populationSize, _threeSATInstance.numberOfVariables), crossoverProb(_crossoverProb), threeSATInstance(_threeSATInstance), hybridVersion(_hybridVersion)
  {
    for (Bitstring& bitstring : population)
    {
      bitstring.MutationLSFI(threeSATInstance);
    }
    timesTwoPointCrossoverWasLocalOptimum = 0;
    timesTwoPointCrossoverWasNotLocalOptimum = 0;

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
        if (hybridVersion == HybridVersion::PartionCrossover)
          newPopulation.emplace_back(Bitstring::GPX(population[p1], population[p2], threeSATInstance));
        else
          newPopulation.emplace_back(Bitstring::TwoPointCrossover(population[p1], population[p2]));
        if (hybridVersion == HybridVersion::TwoPointCrossoverImproved)
        {
          if (newPopulation.back().MutationLSFI(threeSATInstance)) //Two point crossover children are rarely local optima, so this is a good improvement
            timesTwoPointCrossoverWasNotLocalOptimum++;
          else
            timesTwoPointCrossoverWasLocalOptimum++;
          
        }
          
      }
      else
      {
        //Mutation
        Bitstring parent1Bitstring = population[p1];
        parent1Bitstring.Mutation(3.0 / (double)parent1Bitstring.NumberOfBits());
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
        newPopulation[i] = Bitstring((uint32_t)newPopulation[i].NumberOfBits());
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

  uint32_t Hybrid::GetBestFitness()
  {
    uint32_t bestFitness = population[0].GetFitness(threeSATInstance);
    for (uint32_t i = 1; i < population.size(); i++)
    {
      const Bitstring& bitstring = population[i];
      uint32_t fitness = bitstring.GetFitness(threeSATInstance);
      if (fitness > bestFitness)
      {
        bestFitness = fitness;
      }
    }

    return bestFitness;
  }

  BlackBoxGenerational::BlackBoxGenerational(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance)
    : population(populationSize, _threeSATInstance.numberOfVariables), crossoverProb(_crossoverProb), threeSATInstance(_threeSATInstance)
  {
  }

  void BlackBoxGenerational::Iterate(uint32_t iteration)
  {
    uint32_t bestSolutionIndex = GetBestSolution();

    std::vector<Bitstring> newPopulation;
    newPopulation.reserve(population.size());
    while (newPopulation.size() < population.size())
    {
      auto [parent1, parent2] = Selection();
      if (Utils::Random::WithProbability(crossoverProb))
      {
        auto [child1, child2] = Bitstring::OnePointCrossover(population[parent1], population[parent2]);
        child1.Mutation(1.0 / child1.NumberOfBits());
        newPopulation.emplace_back(std::move(child1));
        if (newPopulation.size() < population.size())
        {
          child2.Mutation(1.0 / child2.NumberOfBits());
          newPopulation.emplace_back(std::move(child2));
        }
      }
      else
      {
        Bitstring child1 = population[parent1];
        child1.Mutation(1.0 / child1.NumberOfBits());
        newPopulation.emplace_back(std::move(child1));
        if (newPopulation.size() < population.size())
        {
          Bitstring child2 = population[parent2];
          child2.Mutation(1.0 / child2.NumberOfBits());
          newPopulation.emplace_back(std::move(child2));
        }
      }
    }

    std::swap(newPopulation, population);
  }

  std::pair<uint32_t, uint32_t> BlackBoxGenerational::Selection()
  {
    auto GetParent = [this]() {
      auto [p1, p2] = Utils::Random::GetTwoDistinct(0, (uint32_t)population.size() - 1);
      bool b = Utils::Random::WithProbability(0.5);
      uint32_t p1_unordered = b ? p1 : p2;
      uint32_t p2_unordered = b ? p2 : p1;

      if (population[p1_unordered].GetFitness(threeSATInstance) > population[p2_unordered].GetFitness(threeSATInstance))
        return p1_unordered;
      return p2_unordered;
    };
    

    return { GetParent(), GetParent() };
  }

  uint32_t BlackBoxGenerational::GetBestSolution()
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

  uint32_t BlackBoxGenerational::GetBestFitness()
  {
    uint32_t bestFitness = population[0].GetFitness(threeSATInstance);
    for (uint32_t i = 1; i < population.size(); i++)
    {
      const Bitstring& bitstring = population[i];
      uint32_t fitness = bitstring.GetFitness(threeSATInstance);
      if (fitness > bestFitness)
      {
        bestFitness = fitness;
      }
    }

    return bestFitness;
  }

}