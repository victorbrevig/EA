#include "pch.h"
#include "tspPermutation.h"
#include <array>
#include "Headers\greyBoxEA.h"


template<class T>
inline GreyBoxEA<T>::GreyBoxEA(std::vector<TSPpermutation>& population, unsigned int _maxNumberOfIterations, double _mutationProb, double _crossoverProb)
  : population(population)
{
  maxNumberOfIterations = _maxNumberOfIterations;
  mutationProb = _mutationProb;
  crossoverProb = _crossoverProb;
}

template<class T>
bool GreyBoxEA<T>::iterate(const Graph& graph)
{
  return false;
}