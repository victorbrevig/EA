#include "pch.h"

#include "utils.h"
#include "permutationProblems.h"
#include "bitstringProblems.h"
#include <filesystem>

enum class Job {
  TSP_BLACK_BOX,
  TSP_GRAY_BOX_STANDARD_GPX,
  TSP_GRAY_BOX_CHAINED_GPX,
  TSP_GRAY_BOX_CHAINED_PX,
  TSP_BLACK_BOX_GENERATIONAL,
  SAT3_HYBRID_NO_CROSSOVER,
  SAT3_HYBRID_2POINT,
  SAT3_HYBRID_2POINT_IMPROVED,
  SAT3_HYBRID_GPX,
  SAT3_HYBRID_GPX_EQ_VARS_AND_CLAUSES,
  SAT3_BLACK_BOX_GENERATIONAL
};

BitstringProblems::Result RunBitstringJob(const std::string& file, Job job, const std::string outputFile = "")
{
  uint32_t runningTime = 50; //Seconds
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
  case Job::SAT3_HYBRID_GPX_EQ_VARS_AND_CLAUSES:
    return BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::PartionCrossoverEqualVarsAndClauses, runningTime, outputFile);
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    return BitstringProblems::RunBlackBoxGenerational(file, runningTime, outputFile);
  default:
    break;
  }
}



/*

*/

PermutationProblems::Result RunTSPJob(const std::string& file, Job job, const std::string outputFile = "") {

    uint32_t maxNumberOfGenerations = 10;

    switch (job)
    {
    case Job::TSP_BLACK_BOX:
        return PermutationProblems::RunBlackbox1(file, outputFile);
    case Job::TSP_GRAY_BOX_STANDARD_GPX:
        return PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::GPX_STANDARD, outputFile, maxNumberOfGenerations);
    case Job::TSP_GRAY_BOX_CHAINED_PX:
      return PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::PX_CHAINED, outputFile, maxNumberOfGenerations);
    case Job::TSP_GRAY_BOX_CHAINED_GPX:
        return PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::GPX_CHAINED, outputFile, maxNumberOfGenerations);
    case Job::TSP_BLACK_BOX_GENERATIONAL:
        return PermutationProblems::RunBlackboxGenerational(file, outputFile);
    default:
        break;
    }
}

void RunJob(const std::string& file, Job job, const std::string outputFile = "")
{
  uint32_t bitstringRunningTime = 5; //Seconds
  bitstringRunningTime *= 1000; //To milliseconds

  uint32_t maxNumberOfGenerationsTSP = 10;

  switch (job)
  {
  case Job::TSP_BLACK_BOX:
    PermutationProblems::RunBlackbox1(file, outputFile);
    break;
  case Job::TSP_GRAY_BOX_STANDARD_GPX:
    PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::GPX_STANDARD, outputFile, maxNumberOfGenerationsTSP);
    break;
  case Job::TSP_GRAY_BOX_CHAINED_PX:
    PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::PX_CHAINED, outputFile, maxNumberOfGenerationsTSP);
    break;
  case Job::TSP_GRAY_BOX_CHAINED_GPX:
    PermutationProblems::RunGraybox(file, PermutationProblems::PartitionCrossoverVersion::GPX_CHAINED, outputFile, maxNumberOfGenerationsTSP);
    break;
  case Job::TSP_BLACK_BOX_GENERATIONAL:
    PermutationProblems::RunBlackboxGenerational(file, outputFile);
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
  case Job::SAT3_HYBRID_GPX_EQ_VARS_AND_CLAUSES:
    BitstringProblems::RunHybrid(file, BitstringProblems::HybridVersion::PartionCrossoverEqualVarsAndClauses, bitstringRunningTime, outputFile);
    break;
  case Job::SAT3_BLACK_BOX_GENERATIONAL:
    BitstringProblems::RunBlackBoxGenerational(file, bitstringRunningTime, outputFile);
    break;
  default:
    break;
  }
}

void Run3SATBatch(const std::pair<std::string, std::string>& directories, bool runEqVarsAndClauses = false)
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
  const std::string& outputDirectoryGPXEqVarCl = outputDirectory + "PartitionCrossoverEqVarsAndClauses\\";
  std::filesystem::create_directories(outputDirectoryGPXEqVarCl);
  const std::string& outputDirectoryBlackbox = outputDirectory + "BlackBox\\";
  std::filesystem::create_directories(outputDirectoryBlackbox);
  std::vector<std::string> files = Utils::Files::GetAllFilePathsInDirectory(directory);

  int numberOfFiles = std::min((int)files.size(), 96);

  BitstringProblems::CombinedResult combinedResult;

#pragma omp parallel
  {
#pragma omp single
  {
      Utils::Files::PrintLine("----------------------------------------------");
      Utils::Files::PrintLine("Running 3 SAT Hybrid with No Crossover");
      Utils::Files::PrintLine("Files in directory: " + directory);
      Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryNoCrossOver);
      Utils::Files::PrintLine("Results: ");
  }

#pragma omp for schedule(dynamic)
  for (int i = 0; i < numberOfFiles; i++)
  {
    std::string& file = files[i];
    std::string fileName = Utils::Files::GetNameFromFilePath(file);
    std::string outputFile = outputDirectoryNoCrossOver + fileName;
    BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_NO_CROSSOVER, outputFile);
#pragma omp critical
    {
      combinedResult += result;
      Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
    }
  }

#pragma omp single
  {
    combinedResult.PrintAndClear();


    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running 3 SAT Hybrid with 2-Point Crossover");
    Utils::Files::PrintLine("Files in directory: " + directory);
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectory2Point);
    Utils::Files::PrintLine("Results: ");
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
      combinedResult += result;
      Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
    }
  }


#pragma omp single
  {
    combinedResult.PrintAndClear();



    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running 3 SAT Hybrid with 2-Point Crossover Improved");
    Utils::Files::PrintLine("Files in directory: " + directory);
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectory2PointImproved);
    Utils::Files::PrintLine("Results: ");
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
      combinedResult += result;
      Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
    }
  }


#pragma omp single
  {
    combinedResult.PrintAndClear();


    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running 3 SAT Hybrid with Partition Crossover");
    Utils::Files::PrintLine("Files in directory: " + directory);
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryGPX);
    Utils::Files::PrintLine("Results: ");
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
      combinedResult += result;
      Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
    }
  }

  if (runEqVarsAndClauses)
  {
#pragma omp single
    {
      combinedResult.PrintAndClear();


      Utils::Files::PrintLine("----------------------------------------------");
      Utils::Files::PrintLine("Running 3 SAT Hybrid with Partition Crossover with equal number of vars and clauses");
      Utils::Files::PrintLine("Files in directory: " + directory);
      Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryGPXEqVarCl);
      Utils::Files::PrintLine("Results: ");
    }

#pragma omp for schedule(dynamic)
    for (int i = 0; i < numberOfFiles; i++)
    {
      std::string& file = files[i];
      std::string fileName = Utils::Files::GetNameFromFilePath(file);
      std::string outputFile = outputDirectoryGPXEqVarCl + fileName;
      BitstringProblems::Result result = RunBitstringJob(file, Job::SAT3_HYBRID_GPX_EQ_VARS_AND_CLAUSES, outputFile);
#pragma omp critical
      {
        combinedResult += result;
        Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
      }
    }
  }

#pragma omp single
  {
    combinedResult.PrintAndClear();


    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running 3 SAT Black Box Generational");
    Utils::Files::PrintLine("Files in directory: " + directory);
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryBlackbox);
    Utils::Files::PrintLine("Results: ");
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
      combinedResult += result;
      Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness) + ", Is optimal: " + (result.isOptimal ? "true" : "false"));
    }
  }
  }

  combinedResult.PrintAndClear();
}


void RunTSPBatch(const std::pair<std::string, std::string>& directories, bool alsoGenerational = true, uint32_t runs = 100)
{
    const std::string& file = directories.first;
    const std::string& outputDirectory = Utils::Files::GetWorkingDirectory() + directories.second;

    const std::string& outputDirectoryBlackBox = outputDirectory + "BlackBox\\";
    std::filesystem::create_directories(outputDirectoryBlackBox);
    const std::string& outputDirectoryBlackBoxGenerational = outputDirectory + "BlackBox_Generational\\";
    std::filesystem::create_directories(outputDirectoryBlackBoxGenerational);
    const std::string& outputDirectoryGrayBoxGPX = outputDirectory + "GrayBox_GPX\\";
    std::filesystem::create_directories(outputDirectoryGrayBoxGPX);
    const std::string& outputDirectoryGrayBoxGPXChained = outputDirectory + "GrayBox_GPX_Chained\\";
    std::filesystem::create_directories(outputDirectoryGrayBoxGPXChained);
    const std::string& outputDirectoryGrayBoxPXChained = outputDirectory + "GrayBox_PX_Chained\\";
    std::filesystem::create_directories(outputDirectoryGrayBoxPXChained);


    int numberOfTimesToRun = (int)runs;

    PermutationProblems::CombinedResult combinedResult;
    
#pragma omp parallel
    {

    // BLACKBOX
#pragma omp single
    {
      Utils::Files::PrintLine("----------------------------------------------");
      Utils::Files::PrintLine("Running TSP BlackBox");
      Utils::Files::PrintLine("File name: " + Utils::Files::GetNameFromFilePath(file));
      Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryBlackBox);
      Utils::Files::PrintLine("Results: ");
    }
#pragma omp for
    for (int i = 0; i < numberOfTimesToRun; i++)
    {
        std::string outputFile = outputDirectoryBlackBox + "RunNumber_" + std::to_string(i) + "_" + Utils::Files::GetNameFromFilePath(file);
        PermutationProblems::Result result = RunTSPJob(file, Job::TSP_BLACK_BOX, outputFile);
#pragma omp critical
        {
          combinedResult += result;
          Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness));
        }
    }

    if (alsoGenerational)
    {
#pragma omp single
      {
        combinedResult.PrintAndClear();

        // BLACKBOX GENERATIONAL

        Utils::Files::PrintLine("----------------------------------------------");
        Utils::Files::PrintLine("Running TSP BlackBox Generational");
        Utils::Files::PrintLine("File name: " + Utils::Files::GetNameFromFilePath(file));
        Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryBlackBoxGenerational);
        Utils::Files::PrintLine("Results: ");
      }
#pragma omp for
      for (int i = 0; i < numberOfTimesToRun; i++)
      {
        std::string outputFile = outputDirectoryBlackBoxGenerational + "RunNumber_" + std::to_string(i) + "_" + Utils::Files::GetNameFromFilePath(file);
        PermutationProblems::Result result = RunTSPJob(file, Job::TSP_BLACK_BOX_GENERATIONAL, outputFile);
#pragma omp critical
        {
          combinedResult += result;
          Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness));
        }
      }
    }

    }

    combinedResult.PrintAndClear();

    // GRAYBOX HYBRID STANDARD GPX
    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running TSP GrayBox Hybrid (LK+GPX)");
    Utils::Files::PrintLine("File name: " + Utils::Files::GetNameFromFilePath(file));
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryGrayBoxGPX);
    Utils::Files::PrintLine("Results: ");
    for (int i = 0; i < numberOfTimesToRun; i++)
    {
        std::string outputFile = outputDirectoryGrayBoxGPX + "RunNumber_" + std::to_string(i) + "_" + Utils::Files::GetNameFromFilePath(file);
        PermutationProblems::Result result = RunTSPJob(file, Job::TSP_GRAY_BOX_STANDARD_GPX, outputFile);
        combinedResult += result;
        Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness));
    }
    combinedResult.PrintAndClear();

    // GRAY BOX  CHAINED GPX
    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running TSP Gray Box GPX Chained");
    Utils::Files::PrintLine("File name: " + Utils::Files::GetNameFromFilePath(file));
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryGrayBoxGPXChained);
    Utils::Files::PrintLine("Results: ");
    for (int i = 0; i < numberOfTimesToRun; i++)
    {
        std::string outputFile = outputDirectoryGrayBoxGPXChained + "RunNumber_" + std::to_string(i) + "_" + Utils::Files::GetNameFromFilePath(file);
        PermutationProblems::Result result = RunTSPJob(file, Job::TSP_GRAY_BOX_CHAINED_GPX, outputFile);
        combinedResult += result;
        Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness));
    }
    combinedResult.PrintAndClear();

    // GRAY BOX  CHAINED PX
    Utils::Files::PrintLine("----------------------------------------------");
    Utils::Files::PrintLine("Running TSP Gray Box PX Chained");
    Utils::Files::PrintLine("File name: " + Utils::Files::GetNameFromFilePath(file));
    Utils::Files::PrintLine("Detailed output files in directory: " + outputDirectoryGrayBoxPXChained);
    Utils::Files::PrintLine("Results: ");
    for (int i = 0; i < numberOfTimesToRun; i++)
    {
        std::string outputFile = outputDirectoryGrayBoxPXChained + "RunNumber_" + std::to_string(i) + "_" + Utils::Files::GetNameFromFilePath(file);
        PermutationProblems::Result result = RunTSPJob(file, Job::TSP_GRAY_BOX_CHAINED_PX, outputFile);
        combinedResult += result;
        Utils::Files::PrintLine("Fitness: " + std::to_string(result.bestFitness));
    }
    combinedResult.PrintAndClear();
    


}


int main()
{
  bool runTestSuite = true;

  if (runTestSuite)
  {
    
    struct BatchJobs {
      std::string inputFile;
      std::string outputDir;
      bool runGenerational;
      uint32_t runs;
    };

    auto RunTSPJobs = []() {
        std::vector<BatchJobs> Jobs = {
        {"..\\ALL_tsp\\berlin52.tsp", "..\\OUTPUT\\TSP\\berlin52\\", true, 100 },
        {"..\\ALL_tsp\\bier127.tsp", "..\\OUTPUT\\TSP\\bier127\\", true, 100 },
        {"..\\ALL_tsp\\d493.tsp", "..\\OUTPUT\\TSP\\d493\\", true, 100 },
        {"..\\ALL_tsp\\att532.tsp", "..\\OUTPUT\\TSP\\att532\\", true, 100 },
        {"..\\ALL_tsp\\d657.tsp", "..\\OUTPUT\\TSP\\d657\\", true, 100 },
        {"..\\ALL_tsp\\u1817.tsp", "..\\OUTPUT\\TSP\\u1817\\", false, 100 },
        {"..\\ALL_tsp\\pcb3038.tsp", "..\\OUTPUT\\TSP\\pcb3038", false, 64 },
        {"..\\ALL_tsp\\rl5915.tsp", "..\\OUTPUT\\TSP\\rl5915\\", false, 32 },
        {"..\\ALL_tsp\\usa13509.tsp", "..\\OUTPUT\\TSP\\usa13509\\", false, 8 }
      };

      for (BatchJobs& job : Jobs)
      {
        RunTSPBatch(std::pair<std::string, std::string>(job.inputFile,job.outputDir), job.runGenerational, job.runs);
      }
    };




    auto Run3SATJobs = []() {
      //For 3SAT we run a bunch of problem instances of same size
      //So we can see how many it can solve
      //And we do this for various sizes
      //Each size is in the same directory

      std::vector<std::pair<std::string, std::string>> ThreeSATDirectories = {
        {"..\\ALL_3SAT\\uf20-91\\", "..\\OUTPUT\\3SAT\\uf20-91\\" },
        {"..\\ALL_3SAT\\uf50-218\\", "..\\OUTPUT\\3SAT\\uf50-218\\"},
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
        Run3SATBatch(directory, directory.first == "..\\ALL_3SAT\\uf250-1065\\");
      }
    };

    std::filesystem::remove_all(Utils::Files::GetWorkingDirectory() + "..\\OUTPUT\\"); // Clear output folder

    std::filesystem::create_directories(Utils::Files::GetWorkingDirectory() + "..\\OUTPUT\\");
    Utils::Files::OpenOutputStream(Utils::Files::GetWorkingDirectory() + "..\\OUTPUT\\output.txt");

    Run3SATJobs();
    //RunTSPJobs();

    Utils::Files::CloseOutputStream();
  }
  else
  {
    //Some manual job
    PermutationProblems::RunGraybox("..\\ALL_tsp\\rl5915.tsp", PermutationProblems::PartitionCrossoverVersion::GPX_STANDARD, "", 10, true);
  }

  return 0;
}