#ifndef BITSTRINGPROBLEMS_H
#define BITSTRINGPROBLEMS_H

#include "bitstring.h"

namespace BitstringProblems 
{
  enum class HybridVersion
  {
    PartionCrossover,
    TwoPointCrossover,
    TwoPointCrossoverImproved,
    NoCrossover
  };

  struct Result
  {
    uint32_t bestFitness;
    uint32_t iterations;
    bool isOptimal;
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