#ifndef VEC_H
#define VEC_H

namespace Utils
{
  struct Vec2D
  {
    double x;
    double y;

    double Length();
  };

  Vec2D operator-(const Vec2D& first, const Vec2D& second);
  Vec2D operator+(const Vec2D& first, const Vec2D& second);
  Vec2D operator/(const Vec2D& first, const double second);

  double Distance(const Vec2D& from, const Vec2D& to);
}
#endif