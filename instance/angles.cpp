//
// Created by Dominik Krupke, http://krupke.cc on 12/21/17.
//

#include "angles.h"
namespace angularfreezetag {

double CalculateTurnAngle(double h1, double h2)
{
  if (h1 > h2) std::swap(h1, h2);
  assert(h2 <= 3.15 && h1 >= -3.15);
  auto pi = std::atan(1) * 4;

  if (h1 >= 0 || h2 <= 0) return std::abs(h1 - h2);
  return std::min(std::abs(h1 - h2), std::abs((pi - h2) + (pi + h1)));
}
double CalculateHeading(Point from, Point to)
{
  assert(from.IsValid() && to.IsValid());
  return atan2(to.y-from.y, to.x-from.x);
}
}
