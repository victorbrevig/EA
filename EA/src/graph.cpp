#include "pch.h"
#include "graph.h"
#include "utils.h"

std::vector<GLfloat> Graph::PointsToGLFloats() const
{
  std::vector<GLfloat> ret;
  ret.reserve(points2D.size());

  for(const Utils::Vec2D& point : points2D)
  {
    ret.emplace_back((GLfloat)point.x);
    ret.emplace_back((GLfloat)point.y);
  }

  return ret;
}

std::vector<GLfloat> Graph::PointsToGLFloats(const std::vector<uint32_t>& order) const
{
  std::vector<GLfloat> ret;
  ret.reserve(points2D.size());

  for (uint32_t index : order)
  {
    Utils::Vec2D point = points2D[index];
    ret.emplace_back((GLfloat)point.x);
    ret.emplace_back((GLfloat)point.y);
  }

  return ret;
}

Utils::BoundingBox Graph::GetBoundingBox() const
{
  Utils::BoundingBox ret;

  auto AdjustBoundingBoxToPoint = [&ret](const Utils::Vec2D& point)
  {
    ret.min.x = std::min(point.x, ret.min.x);
    ret.min.y = std::min(point.y, ret.min.y);
    ret.max.x = std::max(point.x, ret.max.x);
    ret.max.y = std::max(point.y, ret.max.y);
  };

  if (points2D.size() > 0)
  {
    ret.min.x = points2D[0].x;
    ret.max.x = points2D[0].x;
    ret.min.y = points2D[0].y;
    ret.max.y = points2D[0].y;
  }

  for(const Utils::Vec2D& point : points2D)
    AdjustBoundingBoxToPoint(point);

  return ret;
}

double Graph::GetEdge(size_t from, size_t to) const
{
  if (edges != nullptr)
    return edges->Get(from, to);

  return Utils::Distance(points2D[from], points2D[to]);
}

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

size_t Graph::GetNumberOfVertices() const
{
  return points2D.size();
}

double Graph::calculateDistByOrder(const std::vector<uint32_t>& order) const
{
    double sum = 0.0;
    // check if edge matrix is available
    if (edges != nullptr) {
        for (unsigned int i = 1; i < order.size(); i++) {
            sum += edges->Get(order[i - 1], order[i]);   
        }
        sum += edges->Get(order[order.size() - 1], order[0]);
    }
    else {
        for (unsigned int i = 1; i < order.size(); i++) {
            Utils::Vec2D firstCoord = points2D[order[i - 1]];
            Utils::Vec2D secondCoord = points2D[order[i]];
            sum += Utils::Distance(firstCoord, secondCoord);
        }
        Utils::Vec2D firstCoord = points2D[order[order.size() - 1]];
        Utils::Vec2D secondCoord = points2D[order[0]];
        sum += Utils::Distance(firstCoord, secondCoord);
    }
    return sum;
}


