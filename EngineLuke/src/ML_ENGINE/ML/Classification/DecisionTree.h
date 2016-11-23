#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "../Dataframe/Dataframe.h"

class IDTVisitor;

class DecisionTree
{
public:
	struct Node
	{
		std::vector<Node*> _children;
		std::string _attributeName;
		std::string _conceptClass;
		std::vector<double> _cutpoints;

		void Walk(IDTVisitor* visitor, bool visit);
	};

public:
	DecisionTree(Dataframe& dataframe, int answerIdx);

	void SetDebugOutput(std::ofstream* o);

	void Build();

	void Walk(IDTVisitor* visitor, bool visit);

private:
	Node* BuildTree(
		const std::vector<Instance*>& instances,
		std::vector<bool>& attNoded);

private:
	Dataframe& _dataframe_original;
	Dataframe _dataframe;
	int _answerIdx;
	Node* _root;

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
