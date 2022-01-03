#include "graph.h"
#include "tspPermutation.h"

class Visualizer {
  const Graph& m_Graph;
  const TSPpermutation& m_Permutation;

  double GetPointSize();
public:
  Visualizer(const Graph& graph, const TSPpermutation& permutation);
  int StartVisualization();
};