//
// Created by Dominik Krupke, http://krupke.cc on 12/21/17.
//

#include <gtest/gtest.h>
#include <cmath>
#include "angles.h"
TEST(angles, CalculateTurnAngle){
  using namespace angularfreezetag;
  auto pi = std::atan(1) * 4;
  ASSERT_NEAR(CalculateTurnAngle(pi, -pi), 0.0, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(pi-0.1, -pi), 0.1, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(-pi, pi), 0.0, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(-pi/2.0, pi/2.0), pi, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(-pi, pi/2), pi*0.5, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(pi, pi/2), pi*0.5, 0.01);
  ASSERT_NEAR(CalculateTurnAngle(-pi, -pi/2), pi*0.5, 0.01);
}

TEST(angles, CalculateHeading){
  using namespace angularfreezetag;
  auto pi = std::atan(1) * 4;
  ASSERT_NEAR(CalculateHeading({0,0}, {1,0}), 0.0, 0.01);
  ASSERT_NEAR(CalculateHeading({0,0}, {0,1}), pi/2, 0.01);
  ASSERT_NEAR(CalculateHeading({0,0}, {-1,0}), pi, 0.01);
  ASSERT_NEAR(CalculateHeading({0,0}, {0,-1}), -0.5*pi, 0.01);
}