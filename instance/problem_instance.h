//
// Created by Dominik Krupke, http://krupke.cc on 12/19/17.
//

#ifndef CODE_PROBLEM_INSTANCE_H
#define CODE_PROBLEM_INSTANCE_H
#include <vector>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <random>
#include <ostream>
#include "point.h"
namespace angularfreezetag {
struct ProblemInstance {
  std::vector<PointWithOrientation> points;
  Point start;

  void LoadFromFile(const std::string &filename)
  {
    points.clear();
    std::ifstream file(filename);
    if (file.is_open()) {
      std::string line;
      while (std::getline(file, line)) {
        if (line.empty()) { continue; }
        if (line.at(0) == '#') { continue; }
        std::vector<std::string> tokens;
        boost::split(tokens, line, boost::is_any_of(" \t;"));
        if (tokens.size() == 3) {
          points.push_back({{std::stod(tokens[0]), std::stod(tokens[1])}, std::stod(tokens[2])});
        }
      }
      start = points.at(0).p;
      file.close();
    }
  }

  void FillRandomly(size_t size)
  {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> coord_distr(0.0, 1.0);
    std::uniform_real_distribution<> orient_distr(-3.14, 3.14);

    for (size_t i = 0; i < size; ++i) {
      points.push_back({{coord_distr(gen), coord_distr(gen)}, orient_distr(gen)});
    }
    start = points.at(0).p;
  }

  friend std::ostream &operator<<(std::ostream &os, const ProblemInstance &instance)
  {
    assert(instance.start == instance.points.at(0).p);
    for (const auto &p: instance.points) {
      os << p.p.x << "\t" << p.p.y << "\t" << p.o << std::endl;
    }
    return os;
  }
};
}
#endif //CODE_PROBLEM_INSTANCE_H
