#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Dataframe.h"

class DecisionTree
{
public:
	DecisionTree(Dataframe& dataframe, int answerIdx);

	void SetDebugOutput(std::ofstream* o);

	void Build();

private:
	struct Node
	{
		std::vector<Node*> _children;
		std::string _attributeName;
		std::string _conceptClass;
	};

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