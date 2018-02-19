#include <iostream>
#include <boost/program_options.hpp>
#include "instance/problem_instance.h"
#include "ip/optimal_solver.h"

namespace po = boost::program_options;

int main(const int argc, const char **argv)
{
  using namespace angularfreezetag;

  po::options_description desc("Allowed options");
  std::string filename;
  std::string output_file;
  size_t size;

  desc.add_options()
      ("help,h", "produce help message")
      ("i", po::value<std::string>(&filename), "Instance file (output if -g is used)")
      ("g", po::value<size_t>(&size), "Generate a random instance with n points.")
      ("output,o", po::value<std::string>(&output_file), "Output file");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);


  //print help
  if (vm.count("help") > 0) {
    std::cout << desc << std::endl;
    return 0;
  }

  ProblemInstance instance;
  if (vm.count("g") > 0) {
    //generate instance
    instance.FillRandomly(size);
    std::ofstream file;
    file.open(filename, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
      file << instance;
      file.close();
    } else {
      std::cerr << "Could not write grid graph to file \"" << filename << "\"" << std::endl;
      std::exit(1);
    }
  } else {
    instance.LoadFromFile(filename);
  }

  std::ofstream log_file;
  log_file.open(output_file, std::ios::app);
  if (log_file.is_open()) {
    using clock = std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using time_unit = std::chrono::milliseconds;

    auto start = clock::now();
    OptimalSolver solver{instance};
    solver.Solve();
    auto end = clock::now();
    auto time = duration_cast<time_unit>(end - start).count();
    log_file << filename << "\t" << instance.points.size() << "\t" << time << "\t" << solver.GetLowerBound() << "\t"
             << solver.GetObjValue() << std::endl;
    return 0;
  }

  return 2;
}