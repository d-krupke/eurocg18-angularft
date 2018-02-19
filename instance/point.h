//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#ifndef CODE_POINT_H
#define CODE_POINT_H
#include <ostream>
namespace angularfreezetag {
struct Point {
  double x = std::numeric_limits<double>::infinity();
  double y = std::numeric_limits<double>::infinity();

  Point() {}
  Point(double x, double y) : x{x}, y{y} {}

  bool IsValid()
  const
  {
    return x != std::numeric_limits<double>::infinity()
        && y != std::numeric_limits<double>::infinity();
  }

  bool operator==(const Point &rhs) const;
  bool operator!=(const Point &rhs) const;
  bool operator<(const Point &rhs) const;
  bool operator>(const Point &rhs) const;
  bool operator<=(const Point &rhs) const;
  bool operator>=(const Point &rhs) const;
  friend std::ostream &operator<<(std::ostream &os, const Point &point);
};
std::ostream &operator<<(std::ostream &os, const Point &point);

class PointHasher {
 public:
  size_t operator()(const Point &p) const
  {
    return std::hash<double>()(p.x) ^ (std::hash<double>()(p.y) + 1);
  }
};

struct PointWithOrientation {
  Point p;
  double o;
};
std::ostream &operator<<(std::ostream &os,
                         const angularfreezetag::PointWithOrientation &pwo);
}



#endif //CODE_POINT_H
