//
// Created by Dominik Krupke, http://krupke.cc on 12/20/17.
//
#include <vector>

#include <gtest/gtest.h>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/directed_graph.hpp>

#include "./boost_graph.h"
#include "./ip_graph.h"

TEST(IpGraph, Foobar){
  using BoostGraph = boost::adjacency_list<boost::vecS,
                                           boost::vecS,
                                           boost::directedS,
                                           boost::property<boost::vertex_name_t,
                                                           int>,
                                           boost::property<boost::edge_weight_t,
                                                           double>>;
  BoostGraph graph;
  auto v1 = boost::add_vertex(graph);
  boost::put(boost::get(boost::vertex_name, graph), v1, 42);
  ASSERT_EQ(boost::get(boost::vertex_name, graph, v1), 42);
  auto v2 = boost::add_vertex(graph);
  auto v3 = boost::add_vertex(graph);
  ASSERT_EQ(boost::num_vertices(graph), 3);
}

TEST(IpGraph, NumVertices){
  using namespace angularfreezetag;
  std::vector<PointWithOrientation> points{{{0.0,0.0},0.0},{{1.0, 0.0}, 0.0}, {{0.5, 0.7}, 0.0}};
  IpGraph ip_graph{points, points.at(0).p};
  ASSERT_EQ(ip_graph.GetNumVertices(), 3+2+2*1);
  ASSERT_EQ(ip_graph.GetNumEdges(), 4+2*1);
}

TEST(IpGraph, NumVerticesCollinear){
  using namespace angularfreezetag;
  std::vector<PointWithOrientation> points{{{0.0,0.0},0.0},{{1.0, 0.0}, 0.0}, {{0.0, 1.0}, 0.0}, {{0.0, -1.0}, 0.0}};
  IpGraph ip_graph{points, points.at(0).p};
  ASSERT_EQ(ip_graph.GetNumVertices(), 4+3+3*2);
  ASSERT_EQ(ip_graph.GetNumEdges(), 9+3*4);
}