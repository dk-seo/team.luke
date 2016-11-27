#include "DecisionTree.h"
#include "../Dataframe/Dataframe.h"
#include "../Utilities/Gain.h"
#include "../Utilities/InstanceCategorizer.h"
#include "../Classification/MultiIntegralDiscretizer.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <unordered_map>

static std::vector<std::vector<int>> ToChildrenEntropyVec(
	const std::vector<InstanceCategorizer>& childCategorizers)
{
	std::vector<std::vector<int>> entropyVecs;
	for (const auto& categorizer : childCategorizers)
		entropyVecs.emplace_back(std::move(categorizer.GetEntropyVector()));
	return std::move(entropyVecs);
}

DecisionTree::Node::Node()
	:_discretizer(nullptr)
{
}

DecisionTree::Node::~Node()
{
}

std::string DecisionTree::Node::GetAnswer(const Instance* instance)
{
	if (_children.empty())
		return _conceptClass;

	std::string childName = _discretizer->Discretize(instance);
	auto childIt = _children.find(childName);
	if(childIt != _children.end())
		return childIt->second->GetAnswer(instance);

	for (childIt = _children.begin(); childIt != _children.end(); ++childIt)
	{
		if (childName < childIt->first)
			return childIt->second->GetAnswer(instance);
	}
	
	return std::prev(childIt)->second->GetAnswer(instance);
}

void DecisionTree::Node::Walk(IDTVisitor* visitor, bool visit)
{
	if (visit && !visitor->Visit(this))
		return;

	for (auto& child : _children)
		child.second->Walk(visitor, true);
}

DecisionTree::DecisionTree(Dataframe& dataframe, int answerIdx)
	: _dataframe(dataframe)
	, _answerIdx(answerIdx)
	, _o(nullptr)
	, _root(nullptr)
{
}

void DecisionTree::SetDebugOutput(std::ofstream* o)
{
	_o = o;
}

DecisionTree::Node* DecisionTree::BuildTree(
	const std::vector<Instance*>& instances,
	std::vector<bool>& attNoded)
{
	// classfy instances by answer classes for parent node entropy vector 
	// calculation
	InstanceCategorizer parentCategorizer(_answerIdx);
	for (auto& instance : instances)
		parentCategorizer.Add(instance);

	// if one or less classes exist, purity is 1 -> leaf node
	if (parentCategorizer.GetClassCount() <= 1)
	{
		Node* node = new Node;
		node->_conceptClass = parentCategorizer.GetClasses()[0];
		node->_childrenCountByConcept[node->_conceptClass] = instances.size();
		return node;
	}

	// extract parent node's entropy vector
	std::vector<int> parentEntropyVec =
		std::move(parentCategorizer.GetEntropyVector());

	// calculate gain per each attribute and find the attribute with the highest
	// gain
	double bestGain = 0;
	int bestGainAttIdx = -1;
	InstanceCategorizer bestGainAttCategorizer(0);
	std::vector<InstanceCategorizer> bestGainAttChildCategorizers;
	std::vector<std::string> bestGainClasses;
	for (int attIdx = 0; attIdx < int(_dataframe.GetAttributeCount()); ++attIdx)
	{
		if (attNoded[attIdx])
			continue;

		// categorize instances by current attribute
		MultiIntegralDiscretizer* discretizer =
			new MultiIntegralDiscretizer(attIdx, _answerIdx);
		discretizer->Build(instances);

		InstanceCategorizer categorizerByAtt(attIdx, discretizer);
		for (auto& instance : instances)
			categorizerByAtt.Add(instance);

		// categorize instances of each attribute clsas by concept
		// and save their states so that children entropy vectors can be
		// calculated
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
				categorizerByAnswer.Add(instance);
		}

		// with classified instances for each class of current attribute,
		// generates children entropy vectors.
		std::vector<std::vector<int>> childrenEntropyVecs =
			std::move(ToChildrenEntropyVec(childAnswerCategorizers));

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
			bestGainAttCategorizer = std::move(categorizerByAtt);
			bestGainAttChildCategorizers = std::move(childAnswerCategorizers);
			bestGainClasses = std::move(classes);
		}
	}

	if (bestGainAttIdx < 0)
	{

		Node* node = new Node;
		auto classes = std::move(parentCategorizer.GetClasses());
		std::string dominatingClass;
		size_t dominatingInstanceCount = 0;
		for (auto& classname : classes)
		{
			size_t instanceCount = parentCategorizer.GetCount(classname);
			node->_childrenCountByConcept[classname] = instanceCount;
			if (dominatingInstanceCount < instanceCount)
			{
				dominatingInstanceCount = instanceCount;
				dominatingClass = classname;
			}
		}

		node->_conceptClass = dominatingClass;
		return node;
	}

	// create a node.
	Node* node = new Node;
	// assign attribute it will use to classify instances
	node->_attributeName = _dataframe.GetAttributeName(bestGainAttIdx);
	node->_discretizer.reset(bestGainAttCategorizer.ReleaseDiscretizer());

	std::vector<std::string> parentClasses = 
		std::move(parentCategorizer.GetClasses());
	for (auto& parentClass : parentClasses)
	{
		node->_childrenCountByConcept[parentClass] =
			parentCategorizer.GetCount(parentClass);
	}

	attNoded[bestGainAttIdx] = true;

	// build child nodes recursively and add them to node.
	for (size_t i = 0; i < bestGainAttChildCategorizers.size(); ++i)
	{
		auto& childCategorizer = bestGainAttChildCategorizers[i];
		auto& classname = bestGainClasses[i];
		std::vector<Instance*> childInstances =
			std::move(childCategorizer.GetInstances());

        std::unique_ptr<Node> childNode;
        childNode.reset(BuildTree(childInstances, attNoded));
		if (childNode)
			node->_children.emplace(classname, std::move(childNode));
	}

	attNoded[bestGainAttIdx] = false;

	// return node.
	return node;
}

void DecisionTree::Build()
{
	std::vector<bool> attMarker(_dataframe.GetAttributeCount(), false);
	attMarker[_answerIdx] = true;
	_root.reset(BuildTree(_dataframe.GetInstances(), attMarker));
}

std::string DecisionTree::Classify(const Instance* instance)
{
	return _root->GetAnswer(instance);
}

void DecisionTree::Walk(IDTVisitor* visitor, bool visit)
{
	if (visit && !visitor->Visit(this))
		return;

	if (_root)
		_root->Walk(visitor, true);
}