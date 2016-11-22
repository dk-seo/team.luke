#pragma once
#include <vector>
#include <ostream>

class Dataframe;
class Instance;

class RecommenderSystem
{
public:
  // rename type of vector layout of unsigned integer
  typedef std::vector<unsigned int> IndexList;

public:
	// initialize with critics dataframe to be used for recommendation
	RecommenderSystem(Dataframe& critics);
	~RecommenderSystem();

	// show process detail if stream is given
	void SetDebugOutput(std::ostream* o);

	// recommend for user and return scores for each product based on critics
	std::vector<double> RecommendFor(Instance* user);

	// recommend for users and return user-score table based on critics
	std::vector<std::vector<double>>
		RecommenderSystem::RecommendFor(const Dataframe& users);

  // recommend winses 
  std::vector<unsigned int> RedcommendWines(const IndexList & userFavors);

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

