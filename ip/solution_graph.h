//
// Created by Dominik Krupke, http://krupke.cc on 1/4/18.
//

#ifndef CODE_SOLUTION_GRAPH_H
#define CODE_SOLUTION_GRAPH_H
#include <vector>
#include <assert.h>
#include <map>
#include <boost/graph/properties.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/directed_graph.hpp>
#include <boost/bimap.hpp>
#include <set>
#include <unordered_map>
#include <iostream>
#include "../instance/point.h"

namespace angularfreezetag {
class SolutionGraph {
  using DGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;
  using DVertex = DGraph::vertex_descriptor;
  using DEdge = DGraph::edge_descriptor;
 public:
  void AddActivation(Point source, Point target, double time) {
    std::cout << source << " activates " << target << " at " << time << std::endl;
    if (points.right.count(source) == 0) {
      auto v = boost::add_vertex(graph);
      points.insert({v, source});
    }
    if (points.right.count(target) == 0) {
      auto v = boost::add_vertex(graph);
      points.insert({v, target});
    }
    assert(activation_time.count(target) == 0);
    activation_time[target] = time;
    boost::add_edge(points.right.at(source), points.right.at(target), graph);
  }

  std::vector<std::vector<Point>> GetCycles() {
    std::set<DEdge> edges;
    for (const auto &e: boost::make_iterator_range(boost::edges(graph))) edges.insert(e);
    while (RemoveLeaveEdges(&edges));
    //the remaining edges are a set of cycles
    std::vector<std::vector<Point>> cycles;
    while (!edges.empty()) {
      cycles.push_back(CollectComponent(*(edges.begin()), &edges));
    }
    std::cout << cycles.size() << " Inter Point Cycles" << std::endl;
    return cycles;
  }

  std::vector<Point> CollectComponent(DEdge e, std::set<DEdge> *edges) {
    edges->erase(e);
    for (const auto &ee: *edges) {
      if (boost::target(e, graph) == boost::source(ee, graph)) {
        auto ret = CollectComponent(ee, edges);
        ret.push_back(points.left.at(boost::source(e, graph)));
        return ret;
      }
    }
    return {points.left.at(boost::source(e, graph))};
  }

  bool RemoveLeaveEdges(std::set<DEdge> *edges) {
    bool removed_edges = false;
    for (const auto &e: *edges) {
      if (IsLeaveEdge(e, *edges)) {
        edges->erase(e);
        removed_edges = true;
      }
    }
    return removed_edges;
  }
  bool IsLeaveEdge(DEdge e, const std::set<DEdge> &edges) {
    const auto &v = boost::target(e, graph);
    for (const auto &n: edges) {
      if (boost::source(n, graph) == v) return false;
    }
    return true;
  }

 private:

  DGraph graph;

  std::unordered_map<Point, double, PointHasher> activation_time;
  boost::bimap<DVertex, Point> points;
};
}
#endif //CODE_SOLUTION_GRAPH_H
