#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>

class Instance;
class Dataframe;

class DecisionTree
{
public:
	DecisionTree(Dataframe& dataframe, int answerIdx);

	// if ofstream is not null then prints all the course of works while clustering to the stream.
	void SetDebugOutput(std::ofstream* o);

	void Build();

private:
	void BuildTree(
		const std::vector<Instance*>& instances,
		std::vector<bool> attMarker,
		int answerIdx,
		const std::string& nodeName,
		const std::string& parentBranchName,
		const std::string& tabs);

private:
	Dataframe& _dataframe;
	int _answerIdx;
	std::ofstream* _o; // output stream for debug
};