//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#include "optimal_solver.h"

namespace angularfreezetag {

bool OptimalSolver::Solve(size_t max_time_s)
{
  cplex_solver_.setParam(IloCplex::TiLim, max_time_s);
  cplex_solver_.setParam(IloCplex::EpInt, 0);
  cplex_solver_.setParam(IloCplex::EpGap, 0);
  cplex_solver_.setParam(IloCplex::EpOpt, 1e-9);
  cplex_solver_.setParam(IloCplex::EpAGap, 0);

  ProhibitInterPointCyclesLazyCallback mycb(cplex_env_, edge_variables_, vertex_variables_, graph_);
  cplex_solver_.use(&mycb);

  auto status = cplex_solver_.solve();
  std::cout << cplex_solver_.getStatus() << std::endl;
  assert(status);
  if (GetLowerBound() == GetObjValue()) {
    while (ProhibitInteriorCyclesOfSolution() > 0 || ProhibitInterPointCycles() > 0) {
      assert(false); //should all be done by the callback now
      cplex_solver_.solve();
      assert(status);
    }
  } else {
    unsolved = (ProhibitInteriorCyclesOfSolution() > 0 || ProhibitInterPointCycles() > 0);
  }
  return status;
}
OptimalSolver::OptimalSolver(const ProblemInstance &instance) : instance_{&instance},
                                                                graph_{instance.points, instance.start},
                                                                cplex_model_{cplex_env_},
                                                                cplex_solver_{cplex_model_}
{
  CreateVariables(graph_);

  for (const auto &e :graph_.Edges()) {
    AddConstraintEdgeTimeDependency(e);
  }

  for (const auto &p: graph_.GetPoints()) {
    const auto &point = p.second;
    if (point.GetPoint() != instance.start) {
      AddConstraintInterPointTimeDependency(point);
      AddConstraintHasToBeCovered(point);
    }
  }

  for (const auto &v: graph_.Vertices()) {
    AddConstraintAtMostOneOutEdge(v);
    if (graph_.GetVertexData(v).IsNeighborHeadingVertex()) {
      AddConstraintOnlyOutIfIn(v);
    }
  }

  AddConstraintTransitionLimitationForCycleExclusion();
  AddConstraintsMinMax();
  AddObjectiveMinMakespan();

}
void OptimalSolver::AddConstraintsMinMax()
{
  for (const auto &p: graph_.GetPoints()) {
    const auto &point = p.second;
    for (const auto &s: point.GetStartHeadingVertices()) {
      if (p.first != instance_->start) {
        cplex_model_.add(time_bound_variable_ >= (vertex_variables_.at(s.second)));
      } else {
        cplex_model_.add(vertex_variables_.at(s.second) == 0);
      }
    }
  }
}
void OptimalSolver::AddObjectiveMinMakespan()
{
  // MIN TIME
  IloExpr obj_expr(cplex_env_);
  obj_expr += time_bound_variable_;
  cplex_model_.add(IloMinimize(cplex_env_, obj_expr));
}
void OptimalSolver::AddConstraintInterPointTimeDependency(const IpPoint &point)
{
  //    START >= SUM HEADINGS TO POINT
  assert(point.GetPoint() != instance_->start);
  IloExpr sum{cplex_env_};
  for (const auto &pp: graph_.GetPoints()) {
    const auto &other_point = pp.second;
    if (point.GetPoint() == other_point.GetPoint()) continue;
    const auto v = other_point.GetNeighbordHeadingVertex(point.GetPoint());
    const auto &var = vertex_variables_.at(v);
    sum += var;
  }
  for (const auto &s: point.GetStartHeadingVertices()) {
    const auto &start_var = vertex_variables_.at(s.second);
    cplex_model_.add(start_var >= sum);
  }
}
void OptimalSolver::AddConstraintEdgeTimeDependency(const Edge e)
{
  auto target_var = vertex_variables_.at(graph_.GetEdgeTarget(e));
  auto source_var = vertex_variables_.at(graph_.GetEdgeSource(e));
  auto edge_var = edge_variables_.at(e);
  double q = 14; //~4*pi
  // TARGET >= SOURCE+WEIGHT if EDGE selected
  cplex_model_.add(target_var >= source_var + graph_.GetEdgeCost(e) + (edge_var * q - q));
}
void OptimalSolver::AddConstraintHasToBeCovered(const IpPoint &point)
{
  //    ALL EDGES LEADING TO A VERTEX HEADING TO POINT == 1
  IloExpr all_edges{cplex_env_};
  for (const auto &e: graph_.Edges()) {
    const auto &target = graph_.GetEdgeTarget(e);
    const auto &vertex_data = graph_.GetVertexData(target);

    if (vertex_data.IsNeighborHeadingVertex()) {
      if (vertex_data.GetHeadingPoint() == point.GetPoint()) {
        all_edges += edge_variables_.at(e);
      }
    }
  }
  cplex_model_.add(all_edges == 1);
}
void OptimalSolver::AddConstraintAtMostOneOutEdge(const Vertex v)
{
  //TODO probably only necessary for start vertices
  IloExpr out_edges{cplex_env_};
  for (const auto &e: graph_.Edges()) {
    //TODO why does boost::out_edges not work?
    if (graph_.GetEdgeSource(e) == v) out_edges += edge_variables_.at(e);
  }
  cplex_model_.add(out_edges <= 1);
}
void OptimalSolver::AddConstraintOnlyOutIfIn(const Vertex v)
{
  //    INCOMING >= OUTGOING
  assert(graph_.GetVertexData(v).IsNeighborHeadingVertex());

  IloExpr sum_in{cplex_env_};
  IloExpr sum_out{cplex_env_};
  for (const auto &e: graph_.Edges()) {
    if (graph_.GetEdgeTarget(e) == v) sum_in += edge_variables_.at(e);
    if (graph_.GetEdgeSource(e) == v) sum_out += edge_variables_.at(e);
  }
  cplex_model_.add(sum_in >= sum_out);
}
void OptimalSolver::AddConstraintTransitionLimitationForCycleExclusion()
{
  auto n = std::distance(graph_.GetPoints().begin(), graph_.GetPoints().end());
  IloExpr edge_sum{cplex_env_};
  for (const auto &e: graph_.Edges()) {
    edge_sum += edge_variables_.at(e);
  }
  cplex_model_.add(edge_sum == (n - 1));
}
size_t OptimalSolver::ProhibitInterPointCycles()
{
  //TODO Delete. Redundant to Callback
  size_t num_cuts = 0;
  std::set<Edge> used_edges;
  for (const auto &p: edge_variables_) {
    if (std::round(cplex_solver_.getValue(p.second)) > 0) {
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
    solution_graph.AddActivation(s, t, cplex_solver_.getValue(vertex_variables_.at(v)));
  }
  for (const auto &c: solution_graph.GetCycles()) {
    IloExpr edge_sum{cplex_env_};
    for (const auto &p: c) {
      edge_sum += edge_variables_.at(activation_edges.at(p));
    }
    assert(c.size() >= 2);
    IloExpr rhs{cplex_env_, (static_cast<double>(c.size()) - 1.0)};
    cplex_model_.add(edge_sum <= rhs);
    num_cuts += 1;
  }
  return num_cuts;
}
size_t OptimalSolver::ProhibitInteriorCyclesOfSolution()
{
  //TODO: Delete. Redundant to callback.
  size_t num_cuts = 0;
  std::set<Edge> used_edges;
  for (const auto &p: edge_variables_) {
    if (std::round(cplex_solver_.getValue(p.second)) > 0) {
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
      IloExpr component_sum{cplex_env_};
      for (const auto &e: component) {
        component_sum += 1 * edge_variables_.at(e);
      }
      IloExpr rhs{cplex_env_, (static_cast<double>(component.size()) - 1.0)};
      cplex_model_.add(component_sum <= rhs);
      num_cuts += 1;
    }
  }
  return num_cuts;
}
void OptimalSolver::CreateVariables(const IpGraph &graph)
{
  //TODO: Delete. Redundant to callback.
  // Create Vertex Variables
  for (const auto &v: graph.Vertices()) {
    vertex_variables_.insert({v, IloNumVar{this->cplex_env_}});
  }
  // Create Edge Variables
  for (const auto &e :graph.Edges()) {
    edge_variables_.insert({e, IloBoolVar{this->cplex_env_}});
  }
  // Create Time Variable
  time_bound_variable_ = IloNumVar{this->cplex_env_};
}
bool OptimalSolver::RecursiveCollectComponent(Edge start_edge,
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

}
