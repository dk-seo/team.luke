#include "CoverageRule.h"
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Dataframe.h"
#include "ClassCounter.h"

bool SubRule::Satisfy(const Instance* instance) const
{
	return instance->GetAttribute(att).AsString() == classValue;
}


struct CoverageInfo
{
	std::unordered_map<std::string, int> classCounter;
	std::unordered_map<std::string, int> totalCounter;
};
struct SubruleInclusiveFunc
{
	SubruleInclusiveFunc(std::vector<SubRule>& subrules) : rules(subrules) {}

	bool operator()(const Instance* instance) const
	{
		if (rules.empty())
			return true;
		for (auto& rule : rules)
		{
			if (!rule.Satisfy(instance))
				return false;
		}
		return true;
	}

	std::vector<SubRule>& rules;
};

struct SubruleExclusiveFunc
{
	SubruleExclusiveFunc(std::vector<SubRule>& subrules) : rules(subrules) {}

	bool operator()(const Instance* instance) const
	{
		if (rules.empty())
			return true;

		for (auto& rule : rules)
		{
			if (!rule.Satisfy(instance))
				return true;
		}
		return false;
	}

	std::vector<SubRule>& rules;
};

CoverageRule::CoverageRule(Dataframe& dataframe, int answerIdx)
	:_dataframe(dataframe)
	,_answerIdx(answerIdx)
	,_o(nullptr)
{
}

void CoverageRule::SetDebugOutput(std::ofstream* o)
{
	_o = o;
}

bool CoverageRule::MakeRule(
	std::vector<Instance*>& instances_original,
	std::string forClass,
	std::vector<SubRule> subrules,
	std::string tabs,
	int ruleNum
)
{
	// calculates coverage for each attribute
	std::vector<ClassCounter> attributeCounters(_dataframe.GetAttributeCount());
	std::vector<ClassCounter> conceptCounters(_dataframe.GetAttributeCount());

	int totalClass = 0;
	for (auto& instance : instances_original)
	{
		if (instance->GetAttribute(_answerIdx).AsString() == forClass)
			++totalClass;
	}
	std::vector<Instance*> instances = std::move(
		Filter(instances_original, SubruleInclusiveFunc(subrules)));

	for (auto& instance : instances)
	{
		for (size_t att = 0; att < _dataframe.GetAttributeCount(); ++att)
		{
			if (att == _answerIdx)
				continue;

			if (instance->GetAttribute(_answerIdx).AsString() == forClass)
				attributeCounters[att].Inc(instance, att);

			conceptCounters[att].Inc(instance, att);
		}
	}

	if (totalClass == 0)
		return true;

	// find the best coverage
	int bestCoverAtt = 0;
	std::string bestCoverClass = "";
	int bestNominator = 0;
	int bestDenominator = 0;
	double bestCoverness = 0;
	for (size_t att = 0; att < _dataframe.GetAttributeCount(); ++att)
	{
		if (att == _answerIdx)
			continue;

		std::vector<std::string> classes = attributeCounters[att].GetClasses();
		for (size_t i = 0; i < classes.size(); ++i)
		{
			int nominator = attributeCounters[att].Get(classes[i]);
			int denominator = conceptCounters[att].Get(classes[i]);
			double coverness = double(nominator) / denominator;

			bool updateBest = false;
			if (bestCoverness == coverness)
			{
				if (bestNominator < nominator)
					updateBest = true;
			}
			else if (bestCoverness < coverness)
			{
				updateBest = true;
			}

			if (updateBest)
			{
				bestCoverAtt = att;
				bestCoverClass = classes[i];
				bestNominator = nominator;
				bestDenominator = denominator;
				bestCoverness = coverness;
			}
			if (_o)
			{
				*_o << tabs << _dataframe.GetAttributeName(att) << " = " << classes[i] << ", then " << forClass
					<< " (" << nominator << "/" << denominator << ")" << std::endl;
			}
		}
	}

	if (_o)
	{
		(*_o).setf(std::ios::fixed);
		(*_o).precision(2);
		(*_o) << std::endl;
		(*_o) << tabs << "Choose : " << _dataframe.GetAttributeName(bestCoverAtt) << " = " << bestCoverClass
			<< "( Highest Coverage : " << bestNominator << "/" << bestDenominator
			<< " = " << double(bestNominator) / bestDenominator << ")" << std::endl << std::endl;
	}

	if (bestNominator == bestDenominator)
	{
		subrules.emplace_back(bestCoverAtt, bestCoverClass);
		if(_o)
		{
			*_o << std::endl;
			*_o << "-- RULE " << ruleNum << " : ";
			for (size_t i = 0; i < subrules.size(); ++i)
			{
				if (i > 0)
					*_o << " and ";

				*_o << _dataframe.GetAttributeName(subrules[i].att) << " = " << subrules[i].classValue;
			}
			*_o << ", then " << forClass << "( " << bestNominator << "/" << totalClass << " Covered )" << std::endl << std::endl;
		}
		std::vector<Instance*> filtered = 
		instances_original = std::move(
			Filter(instances_original, SubruleExclusiveFunc(subrules)));
		return false;
	}
	
	subrules.emplace_back(bestCoverAtt, bestCoverClass);
	return MakeRule(instances_original, forClass, subrules, tabs + "\t", ruleNum);
}

void CoverageRule::Build()
{
	std::vector<Instance*> originalInstances = _dataframe.GetInstances();
	ClassCounter counter;
	for (auto& instance : originalInstances)
		counter.Inc(instance, _answerIdx);

	std::vector<std::string> conceptClasses = std::move(counter.GetClasses());
	int ruleNum = 1;
	for (size_t concept = 0; concept < conceptClasses.size(); ++concept)
	{
		if (_o)
		{
			*_o << "--------" << std::endl;
			*_o << "for " << conceptClasses[concept] << std::endl;
			*_o << std::endl;
		}

		std::vector<Instance*> instances = originalInstances;
		while (!MakeRule(instances, conceptClasses[concept], {}, "\t", ruleNum))
		{
			++ruleNum;
		}

		if (_o)
		{
			*_o << std::endl << std::endl;
		}
	}
}