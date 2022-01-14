#ifndef PERMUTATIONPROBLEMS_H
#define PERMUTATIONPROBLEMS_H

#include "bitstring.h"

namespace PermutationProblems 
{

  struct Result
  {
    uint32_t bestFitness;
    uint32_t iterations;
  };


  Result RunBlackbox1(const std::string& file, const std::string& outputFile);
  Result RunGraybox(const std::string& file, bool modifiedComponentSearch, const std::string& outputFile);
  Result RunBlackboxGenerational(const std::string& file, const std::string& outputFile);

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