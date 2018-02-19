//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#ifndef CODE_IP_POINT_H
#define CODE_IP_POINT_H
#include <unordered_map>

#include <unordered_map>
#include <assert.h>
#include <vector>
#include "../instance/point.h"
#include "./boost_graph.h"

namespace angularfreezetag {

class IpPoint {
 public:

  IpPoint() = default;
  IpPoint(Point p)
  {
    point_ = p;
  }

  void AddStartHeading(double h, Vertex v)
  {
    assert(starts_.count(h)==0);//currently only one start per orientation. Should be changed in the future
    starts_[h]=v;
  }

  void AddHeading(Point ni, Vertex v)
  {
    assert(directed_points_.count(ni)==0);
    directed_points_[ni]=v;
  }

  Vertex GetNeighbordHeadingVertex(Point p) const {
    return directed_points_.at(p);
  }

  Point GetPoint() const { return point_; }

  const auto &GetNeighborHeadingVertices() const { return directed_points_; }

  const auto &GetStartHeadingVertices() const { return starts_; }

  bool operator==(const IpPoint &rhs) const
  {
    return point_ == rhs.point_;
  }

  bool operator!=(const IpPoint &rhs) const
  {
    return !(rhs == *this);
  }
 private:
  Point point_;
  std::unordered_map<Point, Vertex, PointHasher> directed_points_;
  std::unordered_map<double, Vertex> starts_;
};

}
#endif //CODE_IP_POINT_H
