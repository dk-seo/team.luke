#pragma once
#include <vector>
#include <string>
#include "ClassCounter.h"

class Instance;
class Dataframe;


struct SubRule
{
	SubRule(size_t a, std::string c) : att(a), classValue(c) {}

	size_t att;
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
	bool MakeRule(
		std::vector<Instance*>& instances_original,
		std::string forClass,
		std::vector<SubRule> subrules,
		std::string tabs,
		int ruleNum
	);

	int CountClass(const std::vector<Instance*>& instances,
		int index,
		const std::string& forClass);

	void CalculateCoverageInfo(const std::vector<Instance*>& instances,
		const std::vector<SubRule>& rulesSoFar,
		const std::string& answerClass,
		std::vector<ClassCounter>& attributeCounters,
		std::vector<ClassCounter>& conceptCounters);

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