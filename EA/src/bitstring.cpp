#include "pch.h"
#include "bitstring.h"
#include "utils.h"
#include <random>
#include <numeric>

Bitstring::Bitstring() {};

Bitstring::Bitstring(uint32_t size)
  : content(size), indexes(size)
{
  for (uint32_t i = 0; i < size; i++)
  {
    content[i] = Utils::Random::WithProbability(0.5);
  }

  std::iota(std::begin(indexes), std::end(indexes), 0);

  cachedFitnessIsValid = false;
  cachedFitness = 0;
}

uint32_t Bitstring::GetFitness(const ThreeSATInstance& threeSATInstance) const
{
  if (cachedFitnessIsValid)
    return cachedFitness;
  cachedFitness = threeSATInstance.GetSatisfiedClauses(content);
  cachedFitnessIsValid = true;
  return cachedFitness;
}

void Bitstring::Flip(uint32_t index)
{
  content[index] = !content[index];
  cachedFitnessIsValid = false;
}

void Bitstring::Mutation(double mutationRate)
{
  for (uint32_t i = 0; i < content.size(); i++)
  {
    if (Utils::Random::WithProbability(mutationRate))
      Flip(i);
  }
}

void Bitstring::MutationLSFI(const ThreeSATInstance& threeSATInstance)
{
  std::shuffle(indexes.begin(), indexes.end(), Utils::Random::engine);
  bool isLocalOptimum = false;
  while (!isLocalOptimum)
  {
    isLocalOptimum = true;
    uint32_t fitness = GetFitness(threeSATInstance);
    for (uint32_t index : indexes)
    {
      Flip(index);
      uint32_t newFitness = GetFitness(threeSATInstance);
      if (newFitness > fitness)
        fitness = newFitness;
      else
        Flip(index); //Undo
    }
  }
}

uint32_t Bitstring::HammingDistance(const Bitstring& a, const Bitstring& b)
{
  ASSERT(a.content.size() == b.content.size());

  uint32_t hammingDistance = 0;

  for (uint32_t i = 0; i < a.content.size(); i++)
  {
    if (a.content[i] == b.content[i])
      hammingDistance++;
  }

  return hammingDistance;
}