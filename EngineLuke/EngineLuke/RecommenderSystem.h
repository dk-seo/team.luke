#pragma once
#include <vector>
#include <ostream>

class Dataframe;
class Instance;

class RecommenderSystem
{
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

private:
	// return double vector that consists only with attributes of given indices
	std::vector<double> GetFeatureVector(
		Instance* instance, const std::vector<int>& indices);

	// print attributes only with attributes of given indoces
	void PrintAttributes(const std::vector<int>& attIndices);

private:
	Dataframe& _critics; // reference ot critics dataframe
	std::ostream* _o; // debug output stream
};

