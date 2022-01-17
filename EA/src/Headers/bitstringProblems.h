#ifndef BITSTRINGPROBLEMS_H
#define BITSTRINGPROBLEMS_H

#include "bitstring.h"

namespace BitstringProblems 
{
  enum class HybridVersion
  {
    PartionCrossoverEqualVarsAndClauses,
    PartionCrossover,
    TwoPointCrossover,
    TwoPointCrossoverImproved,
    NoCrossover
  };

  struct Result
  {
    Result() 
    {
      bestFitness = 0;
      iterations = 0;
      crossoverCount = 0;
      useCrossoverLocalOptimalCount = false;
      crossoverLocalOptimalCount = 0;
      usePartitionCrossoverComponentCount = false;
      partitionCrossoverMaxComponentCount = 0;
      isOptimal = false;
    }
    uint32_t bestFitness;
    uint32_t iterations;
    uint32_t crossoverCount;
    bool useCrossoverLocalOptimalCount;
    uint32_t crossoverLocalOptimalCount;
    bool usePartitionCrossoverComponentCount;
    uint32_t partitionCrossoverMaxComponentCount;
    bool isOptimal;
  };

  struct CombinedResult
  {
    uint32_t runs = 0;
    std::vector<uint32_t> fitnesses;
    std::vector<uint32_t> iterations;
    uint32_t optimalCount = 0;

    uint32_t crossoverCount = 0;
    bool useCrossoverLocalOptimalCount = false;
    uint32_t crossoverLocalOptimalCount = 0;
    bool usePartitionCrossoverComponentCount = false;
    uint32_t partitionCrossoverMaxComponentCount = 0;

    void operator+=(Result result)
    {
      runs++;
      if (result.isOptimal)
        optimalCount++;
      fitnesses.emplace_back(result.bestFitness);
      iterations.emplace_back(result.iterations);

      crossoverCount += result.crossoverCount;
      if (result.useCrossoverLocalOptimalCount)
      {
        useCrossoverLocalOptimalCount = true;
        crossoverLocalOptimalCount += result.crossoverLocalOptimalCount;
      }
        
      if (result.usePartitionCrossoverComponentCount)
      {
        usePartitionCrossoverComponentCount = true;
        partitionCrossoverMaxComponentCount += result.partitionCrossoverMaxComponentCount;
      }
    }

    void PrintAndClear()
    {
      Utils::Files::PrintLine("-------------");
      Utils::Files::PrintLine("Mean Fitness: " + std::to_string(Utils::Statistic::Mean(fitnesses)) + ", SD: " + std::to_string(Utils::Statistic::StandardDeviation(fitnesses)));
      Utils::Files::PrintLine("Mean Iterations: " + std::to_string(Utils::Statistic::Mean(iterations)) + ", SD: " + std::to_string(Utils::Statistic::StandardDeviation(iterations)));
      Utils::Files::PrintLine("Optimal count: " + std::to_string(optimalCount) + "/" + std::to_string(runs));
      if (useCrossoverLocalOptimalCount)
        Utils::Files::PrintLine("2-point crossover local optimal count: " + std::to_string(crossoverLocalOptimalCount) + "/" + std::to_string(crossoverCount));
      if (usePartitionCrossoverComponentCount)
        Utils::Files::PrintLine("Average number of max components in partition crossover: " + std::to_string((double)partitionCrossoverMaxComponentCount / (double)runs));
      Utils::Files::PrintLine("");
      Utils::Files::PrintLine("");
      runs = 0;
      fitnesses.clear();
      iterations.clear();
      optimalCount = 0;

      crossoverCount = 0;
      useCrossoverLocalOptimalCount = false;
      crossoverLocalOptimalCount = 0;
      usePartitionCrossoverComponentCount = false;
      partitionCrossoverMaxComponentCount = 0;
    }
  };

  bool ShouldStop(long long msStart, uint32_t milliseconds);

  Result RunHybrid(const std::string& file, HybridVersion hybridVersion, uint32_t runningTimeMilliseconds, const std::string& outputFile);
  Result RunBlackBoxGenerational(const std::string& file, uint32_t runningTimeMilliseconds, const std::string& outputFile);

  class Hybrid
  {
    std::vector<Bitstring> population;
    double crossoverProb;
    const ThreeSATInstance& threeSATInstance;
    HybridVersion hybridVersion;

  public:
    //For use with TwoPointCrossoverImproved
    uint32_t timesTwoPointCrossoverWasLocalOptimum;
    uint32_t timesTwoPointCrossoverWasNotLocalOptimum;
    uint32_t crossoverCount;
    uint32_t maxcomponentCount;
    Hybrid(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance, HybridVersion _hybridVersion);
    void Iterate(uint32_t iteration);
    uint32_t GetBestSolution();
    uint32_t GetBestFitness();
    std::pair<uint32_t, uint32_t> Selection();
  };

  class BlackBoxGenerational
  {
    std::vector<Bitstring> population;
    double crossoverProb;
    const ThreeSATInstance& threeSATInstance;
  public:
    BlackBoxGenerational(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance);
    void Iterate(uint32_t iteration);
    uint32_t GetBestSolution();
    std::pair<uint32_t, uint32_t> Selection();
    uint32_t GetBestFitness();
  };
};

#endif