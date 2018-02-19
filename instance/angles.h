//
// Created by Dominik Krupke, http://krupke.cc on 12/21/17.
//

#ifndef CODE_ANGLES_H
#define CODE_ANGLES_H

#include <algorithm>
#include <cmath>
#include <assert.h>
#include "point.h"
namespace angularfreezetag {
double CalculateTurnAngle(double h1, double h2);
double CalculateHeading(Point from, Point to);
}
#endif //CODE_ANGLES_H
