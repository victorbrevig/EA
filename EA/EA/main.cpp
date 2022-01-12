#include "pch.h"

#include "utils.h"
#include "permutationProblems.h"
#include "bitstringProblems.h"

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

void RunJob(const std::string& file, Job job)
{
  uint32_t bitstringRunningTime = 15; //Seconds
  bitstringRunningTime *= 1000; //To milliseconds

  switch (job)
  {
  case Job::TSP_BLACK_BOX:
    PermutationProblems::RunBlackbox1(file);
    break;
  case Job::TSP_GRAY_BOX:
    PermutationProblems::RunGraybox(file);
    break;
  case Job::TSP_BLACK_BOX_GENERATIONAL:
    break;
  case Job::SAT3_HYBRID_NO_CROSSOVER: //Baseline to compare crossover to
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::NoCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_2POINT:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_2POINT_IMPROVED:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossoverImproved, bitstringRunningTime);
    break;
  case Job::SAT3_HYBRID_GPX:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::PartionCrossover, bitstringRunningTime);
    break;
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    BitstringProblems::RunBlackBoxGenerational(file, bitstringRunningTime);
    break;
  default:
    break;
  }
}

void Run3SATBatch(const std::string& directory)
{

}

int main()
{
  bool runTestSuite = false;

  Job job = Job::TSP_GRAY_BOX;

  if (runTestSuite)
  {
    auto RunTSPJobs = []() {
      std::vector<std::string> TSPFiles = {
        {"..\\ALL_tsp\\berlin52.tsp"},
        {"..\\ALL_tsp\\bier127.tsp"},
        {"..\\ALL_tsp\\d493.tsp"},
        {"..\\ALL_tsp\\att532.tsp"},
        {"..\\ALL_tsp\\d657.tsp"},
        {"..\\ALL_tsp\\u1817.tsp"},
        {"..\\ALL_tsp\\pcb3038.tsp"},
        {"..\\ALL_tsp\\rl5915.tsp"},
        {"..\\ALL_tsp\\usa13509.tsp"},
        {"..\\ALL_tsp\\d15112.tsp" }
      };

      for (const std::string& file : TSPFiles)
      {
        RunJob(file, Job::TSP_BLACK_BOX);
        RunJob(file, Job::TSP_GRAY_BOX);
        RunJob(file, Job::TSP_BLACK_BOX_GENERATIONAL);
      }
    };


    auto Run3SATJobs = []() {
      //For 3SAT we run a bunch of problem instances of same size
      //So we can see how many it can solve
      //And we do this for various sizes
      //Each size is in the same directory

      std::vector<std::string> ThreeSATDirectories = {
        {"..\\ALL_3SAT\\uf20-91\\"},
        {"..\\ALL_3SAT\\uf50-218\\"},
        {"..\\ALL_3SAT\\uf75-325\\"},
        {"..\\ALL_3SAT\\uf100-430\\"},
        {"..\\ALL_3SAT\\uf125-538\\"},
        {"..\\ALL_3SAT\\uf150-645\\"},
        {"..\\ALL_3SAT\\uf175-753\\"},
        {"..\\ALL_3SAT\\uf200-860\\"},
        {"..\\ALL_3SAT\\uf225-960\\"},
        {"..\\ALL_3SAT\\uf250-1065\\" }
      };

      for (const std::string& directory : ThreeSATDirectories)
      {
        Run3SATBatch(directory);
      }
    };

    RunTSPJobs();
    Run3SATJobs();
  }
  else
  {

    //Some manual job
    RunJob("..\\ALL_tsp\\pcb3038.tsp", job);
  }

  return 0;
}