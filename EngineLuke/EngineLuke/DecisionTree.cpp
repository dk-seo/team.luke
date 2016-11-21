#include "DecisionTree.h"
#include "Dataframe.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>
#include "Gain.h"
#include "ClassCounter.h"
#include "Dataframe.h"

typedef ClassCounter InstanceCategorizer;

DecisionTree::DecisionTree(Dataframe& dataframe, int answerIdx)
	: _dataframe_original(dataframe)
	, _answerIdx(answerIdx)
	, _o(nullptr)
	, _root(nullptr)
{
}

void DecisionTree::SetDebugOutput(std::ofstream* o)
{
	_o = o;
}

static std::vector<std::string> ExtractClasses(
	const std::vector<Instance*>& instances, size_t att)
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
	Condition(size_t a, std::string v) :att(a), value(v) {}
	bool operator()(const Instance* instance) const
	{
		return instance->GetAttribute(att).AsString() == value;
	}
	size_t att;
	std::string value;
};

static std::vector<std::vector<int>> ToChildrenEntropyVec(
	const std::vector<InstanceCategorizer>& childCategorizers)
{
	std::vector<std::vector<int>> entropyVecs;
	for (const auto& categorizer : childCategorizers)
		entropyVecs.emplace_back(std::move(categorizer.GetEntropyVector()));
	return std::move(entropyVecs);
}

DecisionTree::Node* DecisionTree::BuildTree(
	const std::vector<Instance*>& instances, 
	std::vector<bool>& attNoded)
{
	// classfy instances by answer classes for parent node entropy vector 
	// calculation
	InstanceCategorizer parentCategorizer(_answerIdx);
	for (auto& instance : instances)
		parentCategorizer.Inc(instance);

	// if one or less classes exist, purity is 1 -> leaf node
	if (parentCategorizer.GetClassCount() <= 1)
	{
		Node* node = new Node;
		node->_conceptClass = instances[0]->GetAttribute(_answerIdx).AsString();
		return node;
	}

	// extract parent node's entropy vector
	std::vector<int> parentEntropyVec =
		std::move(parentCategorizer.GetEntropyVector());

	// calculate gain per each attribute and find the attribute with the highest
	// gain
	double bestGain = 0;
	size_t bestGainAttIdx = 0;
	std::vector<InstanceCategorizer> bestGainAttChildCategorizers;
	
	for (size_t attIdx = 0; attIdx < _dataframe.GetAttributeCount(); ++attIdx)
	{
		if (attNoded[attIdx])
			continue;

		// categorize instances by current attribute
		InstanceCategorizer categorizerByAtt(attIdx);
		for (auto& instance : instances)
			categorizerByAtt.Inc(instance);

		// for instances in each attribute class, categorize by concept
		// so that child entropy vector can be calculated
		std::vector<std::string> classes =
			std::move(categorizerByAtt.GetClasses());
		std::vector<InstanceCategorizer> childAnswerCategorizers;
		for (auto& classname : classes)
		{
			std::vector<Instance*> currentClassInstances =
				std::move(categorizerByAtt.GetInstances(classname));

			childAnswerCategorizers.emplace_back(_answerIdx);
			InstanceCategorizer& categorizerByAnswer = 
				childAnswerCategorizers.back();

			for (auto& instance : currentClassInstances)
				categorizerByAnswer.Inc(instance);
		}

		// with classified instances for each class of current attribute,
		// generates children entropy vectors.
		std::vector<std::vector<int>> childrenEntropyVecs = 
			ToChildrenEntropyVec(childAnswerCategorizers);

		// calculate gain with calculated parent/children entropy vectors.s
		double gain = Gain(
			nullptr, _dataframe.GetAttributeName(attIdx), parentEntropyVec, 
			classes, childrenEntropyVecs, true, ""
		);

		// if current gain is better than previous attribute, update.
		if (gain > bestGain)
		{
			bestGain = gain;
			bestGainAttIdx = attIdx;
			bestGainAttChildCategorizers = std::move(childAnswerCategorizers);
		}
	}

	// create a node.
	Node* node = new Node;
	// assign attribute it will use to classify instances
	node->_attributeName = _dataframe.GetAttributeName(bestGainAttIdx);

	attNoded[bestGainAttIdx] = true;

	// build child nodes and add to node.
	for (auto& childCategorizer : bestGainAttChildCategorizers)
	{
		std::vector<Instance*> childInstances =
			std::move(childCategorizer.GetInstances());

		if (Node* childNode = BuildTree(childInstances, attNoded))
			node->_children.push_back(childNode);
	}

	attNoded[bestGainAttIdx] = false;

	// return node.
	return node;
}

void DecisionTree::Build()
{
	_dataframe = std::move(_dataframe_original.Clone());
	std::vector<bool> attMarker(_dataframe.GetAttributeCount(), false);
	attMarker[_answerIdx] = true;
	_root = BuildTree(_dataframe.GetInstances(), attMarker);
}
