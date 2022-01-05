#ifndef LKSEARCH_H
#define LKSEARCH_H
#include "graph.h"
#include "tspPermutation.h"
#include <stack>
#include <queue>
#include <optional>
#include "visualizer.h"

class LKSearch {
  
  struct Flip {
    uint32_t orderFrom;
    uint32_t orderTo;
    uint32_t vertexFrom;
    uint32_t vertexTo;
  };

  struct PromissingVertex {
    PromissingVertex(uint32_t v, double r, bool mm)
      : vertex(v), reward(r), isMakMorton(mm) {};
    uint32_t vertex;
    double reward;
    bool isMakMorton;
  };

  class Compare
  {
  public:
    bool operator() (const LKSearch::PromissingVertex& a, const LKSearch::PromissingVertex& b)
    {
      return a.reward < b.reward;
    }
  };

  const Graph& m_Graph;
  std::vector<uint32_t> m_CurrentOrder;
  std::vector<uint32_t> m_CurrentOrderFromVertex;
  std::stack<Flip> m_FlipSequence;
  uint32_t m_Base;
  Visualizer* m_Visualizer;

  void UpdateOrderFromVertex();

  uint32_t prev(uint32_t vertex) {
    return m_CurrentOrder[m_CurrentOrderFromVertex[vertex] == 0 ? (m_CurrentOrder.size() - 1) : (m_CurrentOrderFromVertex[vertex] - 1)];
  };

  uint32_t next(uint32_t vertex) {
    return m_CurrentOrder[(m_CurrentOrderFromVertex[vertex] + 1) % m_CurrentOrder.size()];
  };

  uint32_t Breadth(uint32_t level);

  std::priority_queue<PromissingVertex, std::vector<PromissingVertex>, LKSearch::Compare> GetLKOrdering(double delta);
  double Step(uint32_t level, double delta);

  void PerformFlipToCurrentTour(Flip flip);
  void AddToFlipSequence(uint32_t from, uint32_t to);
  void DeleteFromFlipSequence();

  struct SearchResult {
    std::vector<uint32_t> new_Tour;
    double improvement;
  };

public:
  LKSearch(const Graph& graph, Visualizer* visualizer);
  SearchResult Search(uint32_t base, const std::vector<uint32_t>& initialTour);
  std::vector<uint32_t> LinKernighan(const TSPpermutation& initialTour);
};

#endif