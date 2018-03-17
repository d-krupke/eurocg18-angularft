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

class ProhibitInterPointCyclesLazyCallback : public IloCplex::LazyConstraintCallbackI {
 public:
  ProhibitInterPointCyclesLazyCallback(const IloEnv &env,
                                       const std::map<Edge, IloBoolVar> &edge_vars,
                                       const std::map<Vertex, IloNumVar> &vertex_variables,
                                       const IpGraph &graph)
      : LazyConstraintCallbackI(env),
        edge_variables_{edge_vars},
        vertex_variables_{vertex_variables},
        graph_{graph} { /* pass */ }

  virtual void main() override
  {
    ProhibitInterPointCycles();
    ProhibitInteriorCyclesOfSolution();
    // return num_cuts;
  }
 protected:
  CallbackI *duplicateCallback() const override
  {
    return (new ProhibitInterPointCyclesLazyCallback(getEnv(), edge_variables_, vertex_variables_, graph_));
  }
 private:
  size_t ProhibitInterPointCycles()
  {
    size_t num_cuts = 0;
    std::set<Edge> used_edges;
    for (const auto &p: edge_variables_) {
      if (std::round(getValue(p.second)) > 0) {
        used_edges.insert(p.first);
      }
    }
    std::unordered_map<Point, Edge, PointHasher> activation_edges;
    SolutionGraph solution_graph;
    for (const auto &e: used_edges) {
      const auto &v = graph_.GetEdgeTarget(e);
      const auto &v_data = graph_.GetVertexData(v);
      assert(v_data.IsNeighborHeadingVertex());
      const auto &s = v_data.GetPoint();
      const auto &t = v_data.GetHeadingPoint();
      // s activates t.
      assert(activation_edges.count(t) == 0);
      activation_edges[t] = e;
      solution_graph.AddActivation(s, t, getValue(vertex_variables_.at(v)));
    }
    for (const auto &c: solution_graph.GetCycles()) {
      IloExpr edge_sum{getEnv()};
      for (const auto &p: c) {
        edge_sum += edge_variables_.at(activation_edges.at(p));
      }
      assert(c.size() >= 2);
      IloExpr rhs{getEnv(), (static_cast<double>(c.size()) - 1.0)};
      add(edge_sum <= rhs);
      num_cuts += 1;
    }
    return num_cuts;
  }
  size_t ProhibitInteriorCyclesOfSolution()
  {
    size_t num_cuts = 0;
    std::set<Edge> used_edges;
    for (const auto &p: edge_variables_) {
      if (std::round(getValue(p.second)) > 0) {
        used_edges.insert(p.first);
      }
    }
    while (!used_edges.empty()) {
      auto e = *(used_edges.begin());
      std::vector<Edge> component;
      component.push_back(e);
      used_edges.erase(e);
      bool is_cycle = RecursiveCollectComponent(e, e, &used_edges, &component);
      if (is_cycle) {
        std::cout << "Found Cycle!" << std::endl;
        for (auto e: component) {
          std::cout << graph_.GetEdgeSource(e) << "->" << graph_.GetEdgeTarget(e) << std::endl;
        }
        // If |E|=|V|, E <= |E|-1
        IloExpr component_sum{getEnv()};
        for (const auto &e: component) {
          component_sum += 1 * edge_variables_.at(e);
        }
        IloExpr rhs{getEnv(), (static_cast<double>(component.size()) - 1.0)};
        add(component_sum <= rhs);
        num_cuts += 1;
      }
    }
    return num_cuts;
  }
  bool RecursiveCollectComponent(Edge start_edge,
                                 Edge current_edge,
                                 std::set<Edge> *used_edges,
                                 std::vector<Edge> *component)
  {
    assert(used_edges->count(start_edge) == 0); //Remove start edge before starting recursion!
    for (auto e: *used_edges) {
      if (graph_.GetEdgeSource(e) == graph_.GetEdgeTarget(current_edge)) {
        component->push_back(e);
        used_edges->erase(e);
        if (graph_.GetEdgeTarget(e) == graph_.GetEdgeSource(start_edge)) return true;
        else return RecursiveCollectComponent(start_edge, e, used_edges, component);
      }
    }
    return false;
  }

  const std::map<Edge, IloBoolVar> &edge_variables_;
  const std::map<Vertex, IloNumVar> &vertex_variables_;
  const IpGraph &graph_;

};
}

#endif //ANGULARFREEZETAG_OPTIMAL_SOLVER_H