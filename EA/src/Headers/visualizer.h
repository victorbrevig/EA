#include "graph.h"

class Visualizer {
  const Graph& m_Graph;
  double GetPointSize();
public:
  Visualizer(const Graph& graph);
  int StartVisualization();
};