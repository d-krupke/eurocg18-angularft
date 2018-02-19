//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#include <gtest/gtest.h>
#include "problem_instance.h"
TEST(ProblemInstance, SimpleTest){
  using namespace angularfreezetag;
  PointWithOrientation p{{1.1, 2.2}, 3.3};
  std::cout << p << std::endl;

  PointHasher hasher;
  std::cout<< hasher(p.p) <<std::endl;
  std::cout << hasher({1.0, 2.0}) <<std::endl;
  std::cout << hasher({2.0, 1.0}) <<std::endl;
}