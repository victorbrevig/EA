#include "pch.h"
#include "graph.h"

void Graph::Add2DCoordinate(double x, double y)
{
  points2D.emplace_back(Utils::Vec2D{ x, y });
}

void Graph::ObtainEdgesFrom2DPoints()
{
  edges = std::make_unique<Utils::Matrix>(points2D.size(), points2D.size());
  for (size_t i = 0; i < points2D.size(); i++)
  {
    for (size_t j = 0; j < points2D.size(); j++)
    {
      edges->Set(i, j, Utils::Distance(points2D[i], points2D[j]));
    }
  }
}