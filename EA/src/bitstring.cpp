#include "pch.h"
#include "bitstring.h"
#include "utils.h"
#include <random>
#include <numeric>
#include <unordered_set>

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

Bitstring::Bitstring(uint32_t size, bool defaultValue)
  : content(size, defaultValue), indexes(size)
{
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

void Bitstring::Setbit(uint32_t index, bool bit)
{
  content[index] = bit;
  cachedFitnessIsValid = false;
}

uint32_t Bitstring::NumberOfBits() const
{
  return (uint32_t)content.size();
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

bool Bitstring::MutationLSFI(const ThreeSATInstance& threeSATInstance)
{
  bool didFindImprovement = false;
  bool isLocalOptimum = false;
  while (!isLocalOptimum)
  {
    std::shuffle(indexes.begin(), indexes.end(), Utils::Random::engine);
    isLocalOptimum = true;
    uint32_t fitness = GetFitness(threeSATInstance);
    for (uint32_t index : indexes)
    {
      Flip(index);
      uint32_t newFitness = GetFitness(threeSATInstance);
      if (newFitness > fitness)
      {
        didFindImprovement = true;
        fitness = newFitness;
      }

      else
        Flip(index); //Undo
    }
  }

  return didFindImprovement;
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

Bitstring Bitstring::TwoPointCrossover(const Bitstring& a, const Bitstring& b)
{
  bool p1IsA = Utils::Random::WithProbability(0.5);
  const Bitstring& p1 = p1IsA ? a : b;
  const Bitstring& p2 = p1IsA ? b : a;

  auto [point1, point2] = Utils::Random::GetTwoDistinct(0, a.NumberOfBits());

  uint32_t bitstringSize = (uint32_t)a.NumberOfBits();

  Bitstring child(bitstringSize);

  for (uint32_t i = 0; i < point1; i++)
    child.Setbit(i, p1.content[i]);

  for (uint32_t i = point1; i < point2; i++)
    child.Setbit(i, p2.content[i]);

  for (uint32_t i = point2; i < bitstringSize; i++)
    child.Setbit(i, p1.content[i]);

  return child;
}

std::pair<Bitstring, Bitstring> Bitstring::OnePointCrossover(const Bitstring& a, const Bitstring& b)
{
  uint32_t point = Utils::Random::GetRange(0, a.NumberOfBits() - 1);

  uint32_t bitstringSize = (uint32_t)a.NumberOfBits();

  Bitstring child1(bitstringSize);
  Bitstring child2(bitstringSize);


  for (uint32_t i = 0; i < point; i++)
  {
    child1.Setbit(i, a.content[i]);
    child2.Setbit(i, b.content[i]);
  }
  for (uint32_t i = point; i < bitstringSize; i++)
  {
    child2.Setbit(i, a.content[i]);
    child1.Setbit(i, b.content[i]);
  }

  return { child1, child2 };
}

Bitstring Bitstring::GPX(const Bitstring& a, const Bitstring& b, const ThreeSATInstance& threeSATInstance)
{
  ASSERT(a.content.size() == b.content.size());

  uint32_t bitstringSize = (uint32_t)a.NumberOfBits();

  Bitstring child(bitstringSize);
  Bitstring f1(bitstringSize, false);
  Bitstring f2(bitstringSize, false);

#ifdef DEBUG
  std::vector<bool> setChildBits(bitstringSize, false);
#endif // DEBUG

  std::vector<bool> uncommonBits(bitstringSize, false);
  std::unordered_set<uint32_t> remainingVertices;
  remainingVertices.reserve(bitstringSize);
  for (uint32_t i = 0; i < bitstringSize; i++)
  {
    if (a.content[i] != b.content[i])
    {
      uncommonBits[i] = true;
      remainingVertices.insert(i);
    }
    else
    {
      child.Setbit(i, a.content[i]);
#ifdef DEBUG
      setChildBits[i] = true;
#endif // DEBUG
    }
  }

  UndirectedGraph variableInteractionGraph = threeSATInstance.GetVariableInteractionGraph();
  UndirectedGraph VIGSharedRemoved(bitstringSize);
  VIGSharedRemoved.ImportEdges(variableInteractionGraph, uncommonBits);



#ifdef DEBUG
  uint32_t numberOfConnectedComponents = 0;
  uint32_t numberOfLargeConnectedComponents = 0;
#endif
  bool choice1 = false;
  bool choice2 = false;
  while (remainingVertices.size() > 0) {
    // take first vertex in remainingVertices as start vertex for BFS
    uint32_t startVertex = *begin(remainingVertices);
    std::vector<uint32_t> connectedComponent = VIGSharedRemoved.BFS(startVertex);

    //All other vertices equals out as they are disconnected from the component
    //So whatever f1 and f2 is they just need to be equal here
    f1 = a;
    f2 = a;

    // remove vertices in connectedComponent from remainingVertices
    for (const auto& v : connectedComponent) {
      std::unordered_set<uint32_t>::iterator it = remainingVertices.find(v);

      remainingVertices.erase(it);

      ASSERT(a.content[v] != b.content[v]);
      f1.Setbit(v, a.content[v]);
      f2.Setbit(v, b.content[v]);
    }

    if (f1.GetFitness(threeSATInstance) > f2.GetFitness(threeSATInstance))
    {
      for (const auto& v : connectedComponent)
      {
        child.Setbit(v, a.content[v]);
#ifdef DEBUG
        setChildBits[v] = true;
#endif // DEBUG
      }
      choice1 = true;
    }
    else 
    {
      for (const auto& v : connectedComponent)
      {
        child.Setbit(v, b.content[v]);
#ifdef DEBUG
        setChildBits[v] = true;
#endif // DEBUG
      }

      if (f1.GetFitness(threeSATInstance) < f2.GetFitness(threeSATInstance))
        choice2 = true;
    }
#ifdef DEBUG
    numberOfConnectedComponents++;

    if (connectedComponent.size() > 1)
      numberOfLargeConnectedComponents++;
#endif
  }
#ifdef DEBUG

  for (uint32_t i = 0; i < bitstringSize; i++)
  {
    ASSERT(setChildBits[i]);
  }


  uint32_t cFitness = child.GetFitness(threeSATInstance);
  uint32_t aFitness = a.GetFitness(threeSATInstance);
  ASSERT(cFitness >= aFitness);
  ASSERT(child.GetFitness(threeSATInstance) >= b.GetFitness(threeSATInstance));

#endif
  return a;
}