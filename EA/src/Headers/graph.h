#ifndef GRAPH_H
#define GRAPH_H
#include "utils.h"
#include "boundingBox.h"
#include <array>

class Graph
{
  const static uint32_t NUMBER_OF_NEAR_NEIGHBORS = 28;
public:
  struct VertexDist
  {
    //This struct can be used to get the distance to a vertex and the vertex itself
    //It is used under the assumption that we know the 'from' vertex
    uint32_t vertex;
    double distance;
  };
private:
  std::vector<Utils::Vec2D> points2D;
  std::unique_ptr<Utils::Matrix> edges = nullptr;

  mutable std::vector<std::array<Graph::VertexDist, NUMBER_OF_NEAR_NEIGHBORS>> m_NearNeighbors;

  
  std::vector<VertexDist> GetOrderedIncidence(uint32_t vertex) const;
public:
  void UpdateNearNeighbors() const;
  std::vector<GLfloat> PointsToGLFloats() const;
  std::vector<GLfloat> PointsToGLFloats(const std::vector<uint32_t>& order) const;
  Utils::BoundingBox GetBoundingBox() const;
  double GetEdge(size_t from, size_t to) const;
  void Add2DCoordinate(double x, double y);
  void ObtainEdgesFrom2DPoints();
  size_t GetNumberOfVertices() const;
  double calculateDistByOrder(const std::vector<uint32_t>& order) const;
  const std::array<Graph::VertexDist, NUMBER_OF_NEAR_NEIGHBORS>& GetNearNeighborsOf(uint32_t vertex) const;
};
#endif