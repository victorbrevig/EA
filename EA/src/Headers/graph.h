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
  std::vector<GLfloat> PointsToGLFloats(const std::vector<uint32_t>& order) const;
  Utils::BoundingBox GetBoundingBox() const;
  double GetEdge(size_t from, size_t to) const;
  void Add2DCoordinate(double x, double y);
  void ObtainEdgesFrom2DPoints();
  size_t GetNumberOfVertices() const;
  double calculateDistByOrder(const std::vector<uint32_t>& order) const;
  double calculateDistBetweenTwoVertices(uint32_t v1, uint32_t v2);
};
#endif