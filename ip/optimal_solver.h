//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//
#ifndef ANGULARFREEZETAG_OPTIMAL_SOLVER_H
#define ANGULARFREEZETAG_OPTIMAL_SOLVER_H

#include <map>
#include <set>
#include <vector>

#include <boost/bimap.hpp>

#include "../instance/problem_instance.h"
#include "./ip_graph.h"
#include "../utils/cplex.hpp"
#include "./solution_graph.h"

namespace angularfreezetag {
class OptimalSolver {
 public:
  // Compute
  bool Solve(size_t max_time_s = 900);

  // Get Solution
  double GetObjValue() { return (unsolved ? 6.28 : cplex_solver_.getObjValue()); }
  double GetLowerBound() { return cplex_solver_.getBestObjValue(); }

  explicit OptimalSolver(const ProblemInstance &instance);

 private:

  void CreateVariables(const IpGraph &graph);
  // Static Constraints
  void AddConstraintsMinMax();
  void AddObjectiveMinMakespan();
  void AddConstraintInterPointTimeDependency(const IpPoint &point);
  void AddConstraintEdgeTimeDependency(const Edge e);
  void AddConstraintHasToBeCovered(const IpPoint &point);
  void AddConstraintAtMostOneOutEdge(const Vertex v);
  void AddConstraintOnlyOutIfIn(const Vertex v);
  void AddConstraintTransitionLimitationForCycleExclusion();

  // Iterative Constraints
  size_t ProhibitInterPointCycles();
  size_t ProhibitInteriorCyclesOfSolution();

  // Auxiliary
  bool RecursiveCollectComponent(Edge start_edge,
                                 Edge current_edge,
                                 std::set<Edge> *used_edges,
                                 std::vector<Edge> *component);

  // --------------------------------------------------------------
  bool unsolved = false;
  const ProblemInstance *instance_;
  IpGraph graph_;

  // Variables
  std::map<Vertex, IloNumVar> vertex_variables_;
  std::map<Edge, IloBoolVar> edge_variables_;
  IloNumVar time_bound_variable_;
  // CPLEX
  IloEnv cplex_env_;
  IloModel cplex_model_;
  IloCplex cplex_solver_;
};

}

#endif //ANGULARFREEZETAG_OPTIMAL_SOLVER_H