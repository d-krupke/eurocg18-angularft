//
// Created by Dominik Krupke, http://krupke.cc on 12/21/17.
//

#ifndef CODE_BOOST_GRAPH_H
#define CODE_BOOST_GRAPH_H

#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/directed_graph.hpp>
#include "../instance/point.h"
#include "../instance/angles.h"

namespace angularfreezetag {


struct VertexData {

  VertexData(Point p, double heading, bool initial_vertex=false) :
      p_{p},
      heading_to_point_{std::numeric_limits<double>::infinity(),
                        std::numeric_limits<double>::infinity()},
      heading_{heading}
  {

  }

  double GetHeading() const { return heading_; }

  bool IsNeighborHeadingVertex() const { return heading_to_point_.IsValid(); }

  Point GetHeadingPoint() const { assert(IsNeighborHeadingVertex()); return heading_to_point_; }

  Point GetPoint() const { return p_; }

  VertexData() : p_{std::numeric_limits<double>::infinity(),
                    std::numeric_limits<double>::infinity()},
                 heading_to_point_{std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::infinity()},
                 heading_{0.0}
  {

  }

  VertexData(Point p, Point h) : p_{p},
                                 heading_to_point_{h},
                                 heading_{CalculateHeading(p, h)}
  {

  }

 private:
  Point p_;
  Point heading_to_point_;
  double heading_;
};

using BoostGraph = boost::adjacency_list<boost::vecS,
                                         boost::vecS,
                                         boost::directedS,
                                         boost::property<boost::vertex_name_t,
                                                         VertexData>,
                                         boost::property<boost::edge_weight_t,
                                                         double>>;
using Vertex = BoostGraph::vertex_descriptor;
using Edge = BoostGraph::edge_descriptor;

}
#endif //CODE_BOOST_GRAPH_H
