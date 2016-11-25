#pragma once
#include <vector>
#include <ostream>

class Dataframe;
class Instance;

class RecommenderSystem
{
  // rename each type
public:

  typedef unsigned char u8;

  typedef std::vector<int> VecS32;

  typedef std::vector<float> VecF32;

  typedef std::vector<double> VecF64;

  typedef std::vector<unsigned int> VecU32;

  typedef VecU32 IndexList;

  enum class WineType : u8 { RED = 0x00, WHITE = 0x01 };

  typedef std::vector<std::pair<unsigned int, WineType>> WineList;

public:
  // initialize with critics dataframe to be used for recommendation
  RecommenderSystem(Dataframe& critics);
  ~RecommenderSystem();

  // show process detail if stream is given
  void SetDebugOutput(std::ostream* o);

  // recommend for user and return scores for each product based on critics
  std::vector<double> RecommendFor(Instance* user);

  // recommend for users and return user-score table based on critics
  std::vector<std::vector<double>> RecommendFor(const Dataframe& users);

  // recommend winses 
  IndexList RecommendFor(const IndexList & userFavors,
    const WineType type = WineType::RED);

  IndexList RecommendFor(const WineList & wines);

private:
  // return double vector that consists only with attributes of given indices
  std::vector<double> GetFeatureVector(
    Instance* instance, const std::vector<int>& indices);

  // print attributes only with attributes of given indoces
  void PrintAttributes(const std::vector<int>& attIndices);

private:
  Dataframe& _critics; // reference of critics dataframe
  std::ostream* _o; // debug output stream
};

