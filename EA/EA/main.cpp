#include "pch.h"

#include "utils.h"
#include "permutationProblems.h"
#include "bitstringProblems.h"
#include <filesystem>

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

BitstringProblems::Result RunBitstringJob(const std::string& file, Job job, const std::string outputFile = "")
{
  uint32_t runningTime = 5; //Seconds
  runningTime *= 1000; //To milliseconds

  switch (job)
  {
  case Job::SAT3_HYBRID_NO_CROSSOVER: //Baseline to compare crossover to
    return BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::NoCrossover, runningTime, outputFile);
  case Job::SAT3_HYBRID_2POINT:
    return BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossover, runningTime, outputFile);
  case Job::SAT3_HYBRID_2POINT_IMPROVED:
    return BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossoverImproved, runningTime, outputFile);
  case Job::SAT3_HYBRID_GPX:
    return BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::PartionCrossover, runningTime, outputFile);
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    return BitstringProblems::RunBlackBoxGenerational(file, runningTime, outputFile);
  default:
    break;
  }
}

void RunJob(const std::string& file, Job job, const std::string outputFile = "")
{
  uint32_t bitstringRunningTime = 5; //Seconds
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
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::NoCrossover, bitstringRunningTime, outputFile);
    break;
  case Job::SAT3_HYBRID_2POINT:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossover, bitstringRunningTime, outputFile);
    break;
  case Job::SAT3_HYBRID_2POINT_IMPROVED:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::TwoPointCrossoverImproved, bitstringRunningTime, outputFile);
    break;
  case Job::SAT3_HYBRID_GPX:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::PartionCrossover, bitstringRunningTime, outputFile);
    break;
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    BitstringProblems::RunBlackBoxGenerational(file, bitstringRunningTime, outputFile);
    break;
  default:
    break;
  }
}

void Run3SATBatch(const std::pair<std::string, std::string>& directories)
{
  const std::string& directory = directories.first;
  const std::string& outputDirectory = Utils::Files::GetWorkingDirectory() + directories.second;

  const std::string& outputDirectoryNoCrossOver = outputDirectory + "NoCrossover\\";
  std::filesystem::create_directories(outputDirectoryNoCrossOver);
  const std::string& outputDirectory2Point = outputDirectory + "2POINT_Crossover\\";
  std::filesystem::create_directories(outputDirectory2Point);
  const std::string& outputDirectory2PointImproved = outputDirectory + "2POINT_OPT_Crossover\\";
  std::filesystem::create_directories(outputDirectory2PointImproved);
  const std::string& outputDirectoryGPX = outputDirectory + "PartitionCrossover\\";
  std::filesystem::create_directories(outputDirectoryGPX);
  const std::string& outputDirectoryBlackbox = outputDirectory + "BlackBox\\";
  std::filesystem::create_directories(outputDirectoryBlackbox);
  std::vector<std::string> files = Utils::Files::GetAllFilePathsInDirectory(directory);

  int numberOfFiles = std::min((int)files.size(), 100);

#pragma omp parallel
  {
#pragma omp single
  {
      std::cout << "----------------------------------------------\n";
      std::cout << "Running 3 SAT Hybrid with No Crossover\n";
      std::cout << "Files in directory: " << directory << "\n";
      std::cout << "Detailed output files in directory: " << outputDirectoryNoCrossOver << "\n";
      std::cout << "Results: \n";
  }


  //100 jobs max
#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectoryNoCrossOver + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_NO_CROSSOVER, outputFile);
#pragma omp critical
    {
      std::cout << "Fitness: " << result.bestFitness << ", Is optimal: " << (result.isOptimal ? "true" : "false") << std::endl;
    }
  }


#pragma omp single
  {
    std::cout << "----------------------------------------------\n";
    std::cout << "Running 3 SAT Hybrid with 2-Point Crossover\n";
    std::cout << "Files in directory: " << directory << "\n";
    std::cout << "Detailed output files in directory: " << outputDirectory2Point << "\n";
    std::cout << "Results: \n";
  }

#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectory2Point + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_2POINT, outputFile);
#pragma omp critical
    {
      std::cout << "Fitness: " << result.bestFitness << ", Is optimal: " << (result.isOptimal ? "true" : "false") << std::endl;
    }
  }


#pragma omp single
  {
    std::cout << "----------------------------------------------\n";
    std::cout << "Running 3 SAT Hybrid with 2-Point Crossover Improved\n";
    std::cout << "Files in directory: " << directory << "\n";
    std::cout << "Detailed output files in directory: " << outputDirectory2PointImproved << "\n";
    std::cout << "Results: \n";
  }

#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectory2PointImproved + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_2POINT_IMPROVED, outputFile);
#pragma omp critical
    {
      std::cout << "Fitness: " << result.bestFitness << ", Is optimal: " << (result.isOptimal ? "true" : "false") << std::endl;
    }
  }


#pragma omp single
  {
    std::cout << "----------------------------------------------\n";
    std::cout << "Running 3 SAT Hybrid with Partition Crossover\n";
    std::cout << "Files in directory: " << directory << "\n";
    std::cout << "Detailed output files in directory: " << outputDirectoryGPX << "\n";
    std::cout << "Results: \n";
  }

#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectoryGPX + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_GPX, outputFile);
#pragma omp critical
    {
      std::cout << "Fitness: " << result.bestFitness << ", Is optimal: " << (result.isOptimal ? "true" : "false") << std::endl;
    }
  }


#pragma omp single
  {
    std::cout << "----------------------------------------------\n";
    std::cout << "Running 3 SAT Black Box Generational\n";
    std::cout << "Files in directory: " << directory << "\n";
    std::cout << "Detailed output files in directory: " << outputDirectoryBlackbox << "\n";
    std::cout << "Results: \n";
  }

#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectoryBlackbox + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_BLACK_BOX_GENERATIONAL, outputFile);
#pragma omp critical
    {
      std::cout << "Fitness: " << result.bestFitness << ", Is optimal: " << (result.isOptimal ? "true" : "false") << std::endl;
    }
  }
  }

}

int main()
{
  bool runTestSuite = true;

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

      std::vector<std::pair<std::string, std::string>> ThreeSATDirectories = {
        {"..\\ALL_3SAT\\uf20-91\\", "..\\OUTPUT\\3SAT\\uf20-91\\" },
        {"..\\ALL_3SAT\\uf50-218\\", "..\\OUTPUT\\3SAT\\uf50-218\\" },
        {"..\\ALL_3SAT\\uf75-325\\", "..\\OUTPUT\\3SAT\\uf75-325\\" },
        {"..\\ALL_3SAT\\uf100-430\\", "..\\OUTPUT\\3SAT\\uf100-430\\" },
        {"..\\ALL_3SAT\\uf125-538\\", "..\\OUTPUT\\3SAT\\uf125-538\\" },
        {"..\\ALL_3SAT\\uf150-645\\", "..\\OUTPUT\\3SAT\\uf150-645\\" },
        {"..\\ALL_3SAT\\uf175-753\\", "..\\OUTPUT\\3SAT\\uf175-753\\" },
        {"..\\ALL_3SAT\\uf200-860\\", "..\\OUTPUT\\3SAT\\uf200-860\\" },
        {"..\\ALL_3SAT\\uf225-960\\", "..\\OUTPUT\\3SAT\\uf225-960\\" },
        {"..\\ALL_3SAT\\uf250-1065\\", "..\\OUTPUT\\3SAT\\uf250-1065\\" }
      };

      for (const auto& directory : ThreeSATDirectories)
      {
        Run3SATBatch(directory);
      }
    };

    std::filesystem::remove_all(Utils::Files::GetWorkingDirectory() + "..\\OUTPUT\\"); // Clear output folder

    Run3SATJobs();
    RunTSPJobs();
  }
  else
  {
    //Some manual job
    RunJob("..\\ALL_tsp\\pcb3038.tsp", Job::TSP_GRAY_BOX);
  }

  return 0;
}