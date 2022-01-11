#include "pch.h"

#include "utils.h"
#include "permutationProblems.h"
#include "bitstringProblems.h"


int main()
{

  enum class Job {
    TSP_BLACK_BOX,
    TSP_GRAY_BOX,
    TSP_BLACK_BOX_GENERATIONAL,
    SAT3_HYBRID_NO_CROSSOVER,
    SAT3_HYBRID_2POINT,
    SAT3_HYBRID_2POINT_IMPROVED,
    SAT3_HYBRID_GPX,
    SAT3_BLACK_BOX_GENERATIONAL
  };

  std::string bitstringFile = "..\\ALL_3SAT\\UF250.1065.100\\uf250-035.cnf";
  uint32_t bitstringRunningTime = 15; //Seconds
  bitstringRunningTime *= 1000; //To milliseconds

  Job job = Job::SAT3_HYBRID_2POINT_IMPROVED;

  switch (job)
  {
  case Job::TSP_BLACK_BOX:
    PermutationProblems::RunBlackbox1("..\\ALL_TSP\\bier127.tsp");
    break;
  case Job::TSP_GRAY_BOX:
    PermutationProblems::RunGraybox("..\\ALL_TSP\\berlin52.tsp");
    break;
  case Job::TSP_BLACK_BOX_GENERATIONAL:
    break;
  case Job::SAT3_HYBRID_NO_CROSSOVER: //Baseline to compare crossover to
    BitstringProblems::RunHybrid(bitstringFile, BitstringProblems::HybridVersion::NoCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_2POINT:
    BitstringProblems::RunHybrid(bitstringFile, BitstringProblems::HybridVersion::TwoPointCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_2POINT_IMPROVED:
    BitstringProblems::RunHybrid(bitstringFile, BitstringProblems::HybridVersion::TwoPointCrossoverImproved, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_GPX:
    BitstringProblems::RunHybrid(bitstringFile, BitstringProblems::HybridVersion::PartionCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    BitstringProblems::RunBlackBoxGenerational(bitstringFile, bitstringRunningTime);
    break;
  default:
    break;
  }

  return 0;
}