#include "RecommenderSystem.h"
#include "Dataframe.h"
#include "Similarity.h"
#include <iostream>



RecommenderSystem::RecommenderSystem(Dataframe& critics)
	:_critics(critics)
	, _o(nullptr)
{
}

RecommenderSystem::~RecommenderSystem()
{
}

void RecommenderSystem::SetDebugOutput(std::ostream* o)
{
	_o = o;
}

// print attribute names of given attribute indices (debug helper function)
void RecommenderSystem::PrintAttributes(const std::vector<int>& attIndices)
{
	for (size_t i = 0; i < attIndices.size(); ++i)
	{
		if (i > 0)
			*_o << ", ";
		*_o << _critics.GetAttributeName(attIndices[i]);
	}
}

std::vector<double> RecommenderSystem::RecommendFor(Instance* user)
{
	if (_o)
	{
		*_o << "For user " <<
			user->GetAttribute(0).AsString() << ": " << std::endl;
	}
	
	// categorize missing/valid attributes
	std::vector<double> productScores;
	std::vector<int> missingAttIndices;
	std::vector<int> validAttIndices;
	int attributeCount = user->GetAttributeCount();
	for (int i = 1; i < attributeCount; ++i) // first column is assumed to be id
	{
		if (user->GetAttribute(i).IsMissing())
		{
			missingAttIndices.emplace_back(i);
			productScores.emplace_back(0.0);
		}
		else
		{
			validAttIndices.emplace_back(i);
			productScores.emplace_back(user->GetAttribute(i).AsDouble());
		}
	}

	// prints categorized missing/valid attributes
	if (_o)
	{
		*_o << "Missing attributes: ";
		PrintAttributes(missingAttIndices);
		*_o << std::endl;
		
		*_o << "Valid attributes: ";
		PrintAttributes(validAttIndices);
		*_o << std::endl;

		*_o << std::endl << std::endl;
	}

	// get user's feature vector that only consists with valid attributes
	// that will be used to calcuate similarity with other critics.
	std::vector<double> userFeatureVec = std::move(
		GetFeatureVector(user, validAttIndices));
	
	// calculate similarity with each critics and save them for score
	// calculation. also, calculate sum of similarites to normalize scores later
	const std::vector<Instance*>& critics = _critics.GetInstances();
	std::vector<double> similarities;
	similarities.reserve(critics.size());

	double sumSimilaritiesInv = 0.0;

	for(auto& critic : critics)
	{
		// get feature vector of a critic that only consists of valid attributes
		std::vector<double> criticFeatureVec = std::move(
			GetFeatureVector(critic, validAttIndices));

		// calculate similarity using euclidean distance similarity.
		similarities.emplace_back(
			EuclideanDistanceSimilarity(userFeatureVec, criticFeatureVec));
		sumSimilaritiesInv += similarities.back();
	}
	sumSimilaritiesInv = 1.0 / sumSimilaritiesInv;

	// variables to find the highest score product for recommendation
	double highestScore = -std::numeric_limits<double>::max();
	int highestScoreIndex = -1;

	// for each missing product, calculate weighted normalized score
	for (size_t i = 0; i < missingAttIndices.size(); ++i)
	{
		int missingAttIdx = missingAttIndices[i];

		if (_o)
		{
			*_o << "For product " <<
				_critics.GetAttributeName(missingAttIdx) << ": " << std::endl;
		}

		// calculate similarity applied scores, sum them.
		double productScore = 0.0;
		for (size_t c = 0; c < critics.size(); ++c)
		{
			double criticScore = 
				critics[c]->GetAttribute(missingAttIdx).AsDouble();
			double simScore = similarities[c] * criticScore;
			productScore += simScore;

			if (_o)
			{
				*_o << "Critic " << critics[c]->GetAttribute(0).AsString()
					<< ": ";
				*_o << "score=" << criticScore << ", ";
				*_o << "similarity=" << similarities[c] << ", ";
				*_o << "score*sim=" << simScore << ", ";
				*_o << std::endl;
			}
		}
		
		// normalize the summation with sum of similarities
		double normalizedScore = productScore * sumSimilaritiesInv;

		// save finalized score for this unscored product
		productScores[missingAttIdx-1] = normalizedScore;
		
		if (_o)
		{
			*_o << "--------------------------------------" << std::endl;
			*_o << "total score: " << productScore << std::endl;;
			*_o << "normalized score: " << normalizedScore << std::endl;
			*_o << std::endl;
		}

		// update highest score product
		if (highestScore < normalizedScore)
		{
			highestScore = normalizedScore;
			highestScoreIndex = missingAttIdx;
		}
	}

	// print final recommendation product for the given user
	if (_o)
	{
		*_o << std::endl;

		*_o << "--------------------------------------" << std::endl;
		*_o << "Recommend: " <<
			_critics.GetAttributeName(highestScoreIndex) << 
			"(score=" << highestScore << ")" << std::endl;
	}

	// return final scores for all the products
	return std::move(productScores);
}

std::vector<std::vector<double>> 
RecommenderSystem::RecommendFor(const Dataframe& testdata)
{
	std::vector<std::vector<double>> userScoreTables;

	// for all users, calculate scores of missing products and save them 
	// to table.
	const std::vector<Instance*>& instances = testdata.GetInstances();
	for (size_t i = 0; i < instances.size(); ++i)
	{
		if (_o && i > 0)
		{
			*_o << std::endl;
			*_o << "======================================" << std::endl;
			*_o << std::endl;
		}
		userScoreTables.emplace_back(std::move(RecommendFor(instances[i])));
	}
	return std::move(userScoreTables);
}

RecommenderSystem::IndexList RecommenderSystem::RecommendFor(const IndexList & userFavors, const WineType type)
{
  IndexList recommended;

  return std::move(recommended);
}

RecommenderSystem::IndexList RecommenderSystem::RecommendFor(const WineList & wines)
{
  IndexList recommend;


  return std::move(recommend);
}


// return double feature vector of given instance that only consists of 
// given attributes of indices
std::vector<double> RecommenderSystem::GetFeatureVector(
	Instance* instance, const std::vector<int>& indices)
{
	std::vector<double> featureVector;
	featureVector.reserve(indices.size());
	for (size_t i = 0; i < indices.size(); ++i)
	{
		featureVector.emplace_back(
			instance->GetAttribute(indices[i]).AsDouble());
	}

	return std::move(featureVector);
}
