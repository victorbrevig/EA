#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H
#include "vec.h"

namespace Utils
{
  struct BoundingBox
  {
    Vec2D min;
    Vec2D max;
    Vec2D GetCenter() { return (min + max) / 2.0; }
    Vec2D GetDimensions() { return max - min; }
  };
}

#endif