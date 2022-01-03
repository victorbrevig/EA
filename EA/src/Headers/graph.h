#ifndef GRAPH_H
#define GRAPH_H
#include "utils.h"
#include "boundingBox.h"

class Graph
{
  std::vector<Utils::Vec2D> points2D;
  std::unique_ptr<Utils::Matrix> edges = nullptr;

public:
  std::vector<GLfloat> PointsToGLFloats() const;
  Utils::BoundingBox GetBoundingBox() const;
  void Add2DCoordinate(double x, double y);
  void ObtainEdgesFrom2DPoints();
  unsigned int GetNumberOfVertices() const;
  double calculateDistByOrder(std::vector<uint32_t>& order) const;
};
#endif