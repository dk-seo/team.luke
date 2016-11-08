#ifndef DK_TYPE_DEF_HPP
#define DK_TYPE_DEF_HPP

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <memory>
#include <limits>
#include <random>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <tuple>

namespace CS399
{
  typedef char  s8;
  typedef short s16;
  typedef int   s32;

  typedef unsigned char  u8;
  typedef unsigned short u16;
  typedef unsigned int   u32;

  typedef float  f32;
  typedef double f64;

  typedef std::ostream        Ostream;
  typedef std::ofstream       Ofstream;
  typedef std::ostringstream  Ostringstream;
  typedef std::fstream        Fstream;
  typedef std::istream        Istream;
  typedef std::ifstream       Ifstream;
  typedef std::istringstream  Istringstream;

  typedef std::string         String;
  typedef std::mt19937        RNG;
}

#endif //typedef.hpp