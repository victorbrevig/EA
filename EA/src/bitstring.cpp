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

Bitstring Bitstring::GPX(const Bitstring& a, const Bitstring& b, const ThreeSATInstance& threeSATInstance)
{
  ASSERT(a.content.size() == b.content.size());

  uint32_t bitstringSize = a.content.size();

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
  }

  UndirectedGraph test(8);
  test.addEdge(0, 5);
  test.addEdge(0, 1);
  test.addEdge(0, 3);
  test.addEdge(0, 7);
  test.addEdge(5, 1);
  test.addEdge(7, 3);
  test.addEdge(7, 6);
  test.addEdge(3, 6);
  test.addEdge(3, 4);
  test.addEdge(3, 1);
  test.addEdge(6, 4);
  test.addEdge(6, 2);
  test.addEdge(4, 2);
  test.addEdge(4, 1);
  test.addEdge(1, 2);

  UndirectedGraph test2(8);
  test2.ImportEdges(test, {1, 0, 0, 0, 1, 1, 1, 0});
  remainingVertices = { 0, 4, 5, 6 };
  //UndirectedGraph variableInteractionGraph = threeSATInstance.GetVariableInteractionGraph();
  //UndirectedGraph VIGSharedRemoved(bitstringSize);
  //VIGSharedRemoved.ImportEdges(variableInteractionGraph, uncommonBits);

  uint32_t numberOfConnectedComponents = 0;
  while (remainingVertices.size() > 0) {
    // take first vertex in remainingVertices as start vertex for BFS
    uint32_t startVertex = *begin(remainingVertices);
    std::vector<uint32_t> connectedComponent = test2.BFS(startVertex);

    // remove vertices in connectedComponent from remainingVertices
    for (const auto& v : connectedComponent) {
      std::unordered_set<uint32_t>::iterator it = remainingVertices.find(v);
      remainingVertices.erase(it);
    }

    if (connectedComponent.size() == 1) {
      // nothing to chose if the connected component is just one vertex
      continue;
    }
    // Only count component if size>1 (effecitively same as fusing common edges)
    numberOfConnectedComponents++;
  }

  return a;
}