//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#include "point.h"

namespace angularfreezetag {
std::ostream &operator<<(
    std::ostream &os,
    const PointWithOrientation &pwo)
{
  os << "x: " << pwo.p.x << " y: " << pwo.p.y << " o: "
     << pwo.o;
  return os;
}
bool Point::operator==(const Point &rhs) const
{
  return x == rhs.x &&
      y == rhs.y;
}
bool Point::operator!=(const Point &rhs) const
{
  return !(rhs == *this);
}
std::ostream &operator<<(std::ostream &os, const Point &point)
{
  os << "x: " << point.x << " y: " << point.y;
  return os;
}
bool Point::operator<(const Point &rhs) const {
  if (x < rhs.x)
    return true;
  if (rhs.x < x)
    return false;
  return y < rhs.y;
}
bool Point::operator>(const Point &rhs) const {
  return rhs < *this;
}
bool Point::operator<=(const Point &rhs) const {
  return !(rhs < *this);
}
bool Point::operator>=(const Point &rhs) const {
  return !(*this < rhs);
}
}