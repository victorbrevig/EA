#ifndef PERMUTATIONPROBLEMS_H
#define PERMUTATIONPROBLEMS_H

#include "bitstring.h"

namespace PermutationProblems 
{
  void RunBlackbox1(const std::string& file);
  void RunGraybox(const std::string& file);

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