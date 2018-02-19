//
// Created by krupke on 23.12.17.
//

#include <gtest/gtest.h>
#include "./point.h"

TEST(Point, Equality){
  using namespace angularfreezetag;
  ASSERT_TRUE(Point(0.0,0.0)==Point(0.0, 0.0));
  ASSERT_FALSE(Point(0.0,0.0)==Point(0.0, -1.0));
}