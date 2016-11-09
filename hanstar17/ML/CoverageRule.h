#pragma once
#include <vector>
#include <string>

class Instance;
class Dataframe;


struct SubRule
{
	SubRule(int a, std::string c) : att(a), classValue(c) {}

	int att;
	std::string classValue;

	bool Satisfy(const Instance* instance) const;
};


class CoverageRule
{
public:
	CoverageRule(Dataframe& dataframe, int answerIdx);

	// if ofstream is not null then prints all the course of works while clustering to the stream.
	void SetDebugOutput(std::ofstream* o);

	void Build();

private:
	bool CoverageRule::MakeRule(
		std::vector<Instance*>& instances_original,
		std::string forClass,
		std::vector<SubRule> subrules,
		std::string tabs,
		int ruleNum
	);

private:
	struct AttributeMetadata {
		std::string name;
		std::vector<std::string> possibleValues;
	};
	std::vector<AttributeMetadata> _attributes;

private:
	Dataframe& _dataframe;
	int _answerIdx;
	std::ofstream* _o; // output stream for debug
};