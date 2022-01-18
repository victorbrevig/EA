#ifndef PERMUTATIONPROBLEMS_H
#define PERMUTATIONPROBLEMS_H

#include "bitstring.h"
#include "visualizer.h"

namespace PermutationProblems 
{
  enum class PartitionCrossoverVersion {
    GPX_STANDARD,
    PX_CHAINED,
    GPX_CHAINED
  };
  void RunBlackbox1(const std::string& file);
  void RunGraybox(const std::string& file, PartitionCrossoverVersion crossoverVersion);
  void RunBlackboxGenerational(const std::string& file);

  struct Result
  {
    uint32_t bestFitness;
    bool usePartitionCrossover;
    std::vector<uint32_t> partitionCrossoverTwoCostComponents;
    std::vector<uint32_t> partitionCrossoverChoices;
  };

  struct CombinedResult
  {
      uint32_t runs = 0;
      std::vector<uint32_t> fitnesses;
      std::vector<uint32_t> partitionCrossoverTwoCostComponents;
      std::vector<uint32_t> partitionCrossoverChoices;
      void operator+=(Result result)
      {
          runs++;
          fitnesses.emplace_back(result.bestFitness);
          if (result.usePartitionCrossover)
          {
            partitionCrossoverTwoCostComponents.insert(partitionCrossoverTwoCostComponents.end(), result.partitionCrossoverTwoCostComponents.begin(), result.partitionCrossoverTwoCostComponents.end());
            partitionCrossoverChoices.insert(partitionCrossoverChoices.end(), result.partitionCrossoverChoices.begin(), result.partitionCrossoverChoices.end());
          }
      }

      void PrintAndClear()
      {
          Utils::Files::PrintLine("-------------");
          Utils::Files::PrintLine("Runs: " + std::to_string(fitnesses.size()));
          Utils::Files::PrintLine("Mean Fitness: " + std::to_string(Utils::Statistic::Mean(fitnesses)) + ", SD: " + std::to_string(Utils::Statistic::StandardDeviation(fitnesses)));
          if (partitionCrossoverTwoCostComponents.size() > 0)
          {
            Utils::Files::PrintLine("Mean number of 2 cost components: " + std::to_string(Utils::Statistic::Mean(partitionCrossoverTwoCostComponents)) + ", SD: " + std::to_string(Utils::Statistic::StandardDeviation(partitionCrossoverTwoCostComponents)));
          }
          if (partitionCrossoverChoices.size() > 0)
          {
            Utils::Files::PrintLine("Mean number of sub-tour choices: " + std::to_string(Utils::Statistic::Mean(partitionCrossoverChoices)) + ", SD: " + std::to_string(Utils::Statistic::StandardDeviation(partitionCrossoverChoices)));
          }
          Utils::Files::PrintLine("");
          Utils::Files::PrintLine("");
          runs = 0;
          fitnesses.clear();
          partitionCrossoverTwoCostComponents.clear();
          partitionCrossoverChoices.clear();
      }
  };


  Result RunBlackbox1(const std::string& file, const std::string& outputFile, bool visualize = false);
  Result RunGraybox(const std::string& file, PartitionCrossoverVersion crossoverVersion, const std::string& outputFile, const uint32_t maxNumberOfGenerations, bool visualize = false);
  Result RunBlackboxGenerational(const std::string& file, const std::string& outputFile, bool visualize = false);

  class Hybrid
  {
    std::vector<Bitstring> population;
    double crossoverProb;
    const ThreeSATInstance& threeSATInstance;
  public:
    Hybrid(uint32_t populationSize, double _crossoverProb, const ThreeSATInstance& _threeSATInstance);
    void Iterate(uint32_t iteration);
    uint32_t GetBestSolution();
    std::pair<uint32_t, uint32_t> Selection();

    void PrintBestSolution();
  };
};

#endif