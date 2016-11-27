#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <map>
#include <memory>
#include "ML/Dataframe/Dataframe.h"

class IDTVisitor;
class MultiIntegralDiscretizer;

class DecisionTree
{
public:
	struct Node
	{
		std::map<std::string, std::unique_ptr<Node>> _children;
		std::map<std::string, size_t> _childrenCountByConcept;
		std::string _attributeName;
		std::string _conceptClass; // only leaf node has this value
		std::string _attributePrefix; // for visualization.. this will be range
		std::unique_ptr<MultiIntegralDiscretizer> _discretizer;

		Node();
		~Node();

		std::string GetAnswer(const Instance* instance);

		void Walk(IDTVisitor* visitor, bool visit);
	};

public:
	DecisionTree(Dataframe& dataframe, int answerIdx);

	void SetDebugOutput(std::ofstream* o);

	void Build();

	std::string Classify(const Instance* instance);

	void Walk(IDTVisitor* visitor, bool visit);

	Node* GetRoot() { return _root.get(); }

private:
	Node* BuildTree(
		const std::vector<Instance*>& instances,
		std::vector<bool>& attNoded);

private:
	Dataframe& _dataframe;
	int _answerIdx;
	std::unique_ptr<Node> _root;

	std::ofstream* _o; // output stream for debug
};


class IDTVisitor
{
public:
	virtual ~IDTVisitor() {}

	// return true means continue visiting into depth
	virtual bool Visit(DecisionTree* tree) { return true; }
	virtual bool Visit(typename DecisionTree::Node* node) { return true; }
};