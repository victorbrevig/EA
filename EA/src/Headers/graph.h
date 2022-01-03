#ifndef GRAPH_H
#define GRAPH_H
#include "utils.h"

class Graph
{
  std::vector<Utils::Vec2D> points2D;
  std::unique_ptr<Utils::Matrix> edges = nullptr;

public:
  void Add2DCoordinate(double x, double y);
  void ObtainEdgesFrom2DPoints();
};
#endif