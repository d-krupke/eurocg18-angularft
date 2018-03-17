//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#include <gtest/gtest.h>
#include "../instance/problem_instance.h"
#include "optimal_solver.h"
TEST(OptimalSolver, SimpleRotation)
{
  using namespace angularfreezetag;
  ProblemInstance instance{{PointWithOrientation{{0.0, 1.0}, 0.0}, PointWithOrientation{{0.0, 0.0}, 0.0}}, {0.0, 0.0}};
  OptimalSolver solver{instance};
  solver.Solve();
  auto value = solver.GetObjValue();
  ASSERT_NEAR(value, 3.14 / 2, 0.1); //90deg
}

TEST(OptimalSolver, MoreRotations)
{
  using namespace angularfreezetag;
  ProblemInstance instance{{PointWithOrientation{{0.0, 1.0}, 0.0}, PointWithOrientation{{0.0, 0.0}, 0.0},
                            PointWithOrientation{{0.0, -1.0}, 0.0}}, {0.0, 0.0}};
  OptimalSolver solver{instance};
  solver.Solve();
  auto value = solver.GetObjValue();
  ASSERT_NEAR(value, 3.14, 0.1); //90deg
}

TEST(OptimalSolver, DegeneratedLineInstance)
{
  using namespace angularfreezetag;
  auto pi = std::atan(1) * 4;
  ProblemInstance instance{{PointWithOrientation{{0.0, 0.0}, pi / 2},
                            PointWithOrientation{{-1.0, 0.0}, 0.0},
                            PointWithOrientation{{-2.0, 0.0}, 0.0},
                            PointWithOrientation{{-3.0, 0.0}, 0.0},
                            PointWithOrientation{{1.0, 0.0}, pi},
                            PointWithOrientation{{2.0, 0.0}, pi},
                            PointWithOrientation{{3.0, 0.0}, pi},
                           }, {0.0, 0.0}};
  OptimalSolver solver{instance};
  solver.Solve();
  auto value = solver.GetObjValue();
  ASSERT_NEAR(value, pi / 2, 0.1);
}

TEST(OptimalSolver, CplexFoobar)
{
  IloEnv cplex_env;
  IloModel cplex_model{cplex_env};
  IloCplex cplex_solver{cplex_model};

  IloNumVar x1{cplex_env};
  IloNumVar x2{cplex_env};

  cplex_model.add(x1 <= x2);
  cplex_model.add(x2 <= 3.0);
  IloExpr obj_expr{cplex_env};
  obj_expr += x1;
  cplex_model.add(IloMaximize(cplex_env, obj_expr));
  cplex_solver.solve();
  std::cout << cplex_solver.getObjValue() << std::endl;
}

TEST(OptimalSolver, RandomLarger)
{
  using namespace angularfreezetag;
  ProblemInstance instance;
  instance.FillRandomly(7);
  OptimalSolver solver{instance};
  solver.Solve();
  auto value = solver.GetObjValue();
  std::cout << "Optimal solution is " << value << std::endl;
  //ASSERT_NEAR(value, 3.14, 0.1); //90deg
}