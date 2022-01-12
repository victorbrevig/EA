#include "pch.h"
#include "vec.h"

namespace Utils
{
  double Vec2D::Length()
  {
    return sqrt(x * x + y * y);
  }

  Vec2D operator-(const Vec2D& first, const Vec2D& second)
  {
    return { first.x - second.x, first.y - second.y };
  }

  Vec2D operator+(const Vec2D& first, const Vec2D& second)
  {
    return { first.x + second.x, first.y + second.y };
  }

  Vec2D operator/(const Vec2D& first, const double second)
  {
    return { first.x / second, first.y / second };
  }

  int Distance(const Vec2D& from, const Vec2D& to)
  {
    Vec2D diff = to - from;
    return (int)(diff.Length() + 0.5); //TSPLib uses rounded distances, so we do so for better comparison
  }
}