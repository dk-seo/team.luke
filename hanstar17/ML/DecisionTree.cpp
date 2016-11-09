#include "DecisionTree.h"
#include "Dataframe.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>
#include "Gain.h"
#include "ClassCounter.h"
#include "Dataframe.h"

DecisionTree::DecisionTree(Dataframe& dataframe, int answerIdx)
	:_dataframe(dataframe)
	,_answerIdx(answerIdx)
	,_o(nullptr)
{
}

void DecisionTree::SetDebugOutput(std::ofstream* o)
{
	_o = o;
}

std::vector<std::string> GetAttributeClasses(
	const std::vector<Instance*>& instances, 
	int att)
{
	std::set< std::string> set;
	for (auto& i : instances)
		set.insert(i->GetAttribute(att).AsString());

	std::vector<std::string> classes;
	for (auto& i : set)
		classes.emplace_back(i);

	return std::move(classes);
}

struct Condition
{
	Condition(int a, std::string v) :att(a), value(v) {}
	bool operator()(const Instance* instance) const
	{
		return instance->GetAttribute(att).AsString() == value;
	}
	int att;
	std::string value;
};

void DecisionTree::BuildTree(const std::vector<Instance*>& instances, 
	std::vector<bool> attMarker, 
	int answerIdx,
	const std::string& nodeName, 
	const std::string& parentBranchName, 
	const std::string& tabs)
{
	// my node information
	ClassCounter parentCounter;
	for (auto& instance : instances)
		parentCounter.Inc(instance, answerIdx);
	std::vector<int> parentV = std::move(parentCounter.GetVector());

	if (_o)
	{
		(*_o).setf(std::ios::fixed);
		(*_o).precision(2);

		*_o << tabs << "------" << std::endl;
		*_o << tabs << nodeName << "=" << parentBranchName << std::endl;
	}

	if (parentV.size() <= 1)
	{
		if (_o)
		{
			*_o << tabs << "* Leaf Node(" << instances[0]->GetAttribute(answerIdx).AsString() << ", " << instances.size() << ")" << std::endl;
		}

		return;
	}

	// calculate children and per each attribute and find the best attribute
	int bestGainIdx = 0;
	double bestGain = 0;
	std::vector<std::string> bestAttributeClasses;
	typedef std::unordered_map<std::string, ClassCounter> ChildClassCounter;
	std::vector<ChildClassCounter> bestChildrenCounters;
	for (size_t i = 0; i < _dataframe.GetAttributeCount(); ++i)
	{
		if (attMarker[i])
			continue;

		std::vector<std::string> attClasses = std::move(
			GetAttributeClasses(instances, i)
		);

		std::vector<std::vector<int>> childVectors;
		std::vector<ChildClassCounter> childrenCounters;
		for (size_t j = 0; j < attClasses.size(); ++j)
		{
			std::vector<Instance*> filtered = std::move(
				Filter(instances, Condition(i, attClasses[j]))
			);
			ChildClassCounter childCounter;
			for (auto& instance : filtered)
				childCounter[attClasses[j]].Inc(instance, answerIdx);

			childrenCounters.emplace_back(childCounter);
			childVectors.push_back({});
			childVectors.back() = std::move(
				childCounter[attClasses[j]].GetVector()
			);
		}

		double gain = Gain(_o, _dataframe.GetAttributeName(i), parentV, attClasses, childVectors, true, tabs);
		if (gain > bestGain)
		{
			bestGain = gain;
			bestGainIdx = i;
			bestAttributeClasses = attClasses;
			bestChildrenCounters = childrenCounters;
		}
	}
	
	if (_o)
	{
		*_o << std::endl;
		*_o << tabs << "Choose: " << _dataframe.GetAttributeName(bestGainIdx) << "(" << bestGain << ")" << std::endl << std::endl;
	}

	attMarker[bestGainIdx] = true;
	for (size_t i = 0; i < bestChildrenCounters.size(); ++i)
	{
		std::vector<Instance*> instances =
			bestChildrenCounters[i][bestAttributeClasses[i]].GetInstances();

		BuildTree(instances, attMarker, answerIdx, _dataframe.GetAttributeName(bestGainIdx), bestAttributeClasses[i], tabs + "\t");
	}
}

void DecisionTree::Build()
{
	std::vector<bool> attMarker(_dataframe.GetAttributeCount(), false);
	attMarker[_answerIdx] = true;
	BuildTree(_dataframe.GetInstances(), attMarker, _answerIdx, "ROOT", "", "");
}
