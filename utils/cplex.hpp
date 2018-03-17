#ifndef CF_SOLVE_CPLEX_HPP
#define CF_SOLVE_CPLEX_HPP

/* needed for some MAC versions of CPLEX */
#include <cstring>
#include <sstream>
#include <chrono>
#include <vector>

#define IL_STD

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wignored-attributes"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4512)
#endif

#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#ifndef CPLEX_HPP_NO_BOOST

#include <boost/config.hpp>

#define CPLEX_HPP_NORETURN BOOST_NORETURN
#else
#define CPLEX_HPP_NORETURN
#endif



#endif //CF_SOLVE_CPLEX_HPP
