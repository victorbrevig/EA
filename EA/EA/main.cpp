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
    SAT3__BLACK_BOX,
    SAT3__GRAY_BOX,
    SAT3__BLACK_BOX_GENERATIONAL
  };

  Job job = Job::TSP_GRAY_BOX;

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
  case Job::SAT3__BLACK_BOX:
    break;
  case Job::SAT3__GRAY_BOX:
    BitstringProblems::RunGraybox("..\\ALL_3SAT\\UF250.1065.100\\uf250-03.cnf");
    break;
  case Job::SAT3__BLACK_BOX_GENERATIONAL:
    break;
  default:
    break;
  }

  return 0;
}