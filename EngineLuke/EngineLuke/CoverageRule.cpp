#include "CoverageRule.h"
#include <vector>
#include <unordered_map>
#include <fstream>
#include "Dataframe.h"

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
	SubruleInclusiveFunc(const std::vector<SubRule>& subrules)
		: rules(subrules) {}

	bool operator()(const Instance* instance) const
	{
		if (rules.empty())
			return true;
		for (const auto& rule : rules)
		{
			if (!rule.Satisfy(instance))
				return false;
		}
		return true;
	}

	const std::vector<SubRule>& rules;
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

int CoverageRule::CountClass(const std::vector<Instance*>& instances,
	int index,
	const std::string& forClass)
{
	int count = 0;
	for (const auto& instance : instances)
	{
		if (instance->GetAttribute(index).AsString() == forClass)
			++count;
	}
	return count;
}

void CoverageRule::CalculateCoverageInfo(
	const std::vector<Instance*>& instances,
	const std::vector<SubRule>& rulesSoFar,
	const std::string& answerClass,
	std::vector<InstanceCategorizer>& attributeCounters,
	std::vector<InstanceCategorizer>& conceptCounters)
{
	std::vector<Instance*> filteredInstances = std::move(
		Filter(instances, SubruleInclusiveFunc(rulesSoFar)));
	for (auto& instance : instances)
	{
		for (size_t att = 0; att < _dataframe.GetAttributeCount(); ++att)
		{
			if (att == _answerIdx)
				continue;

			if (instance->GetAttribute(_answerIdx).AsString() == answerClass)
				attributeCounters[att].Add(instance);

			conceptCounters[att].Add(instance);
		}
	}
}

bool CoverageRule::MakeRule(
	std::vector<Instance*>& instances_original,
	std::string answerClass,
	std::vector<SubRule> rulesSoFar,
	std::string tabs,
	int ruleNum
)
{
	// if give class is entirely covered, finish rules for the class.
	int totalAnswerClassCount = CountClass(
		instances_original, _answerIdx, answerClass);

	if (totalAnswerClassCount == 0)
		return true;

	// calculates coverage Info for every attribute
	std::vector<InstanceCategorizer> attributeCounters;
	attributeCounters.reserve(_dataframe.GetAttributeCount());
	std::vector<InstanceCategorizer> conceptCounters;
	conceptCounters.reserve(_dataframe.GetAttributeCount());
	for (size_t i = 0; i < _dataframe.GetAttributeCount(); ++i)
	{
		attributeCounters.emplace_back(i);
		conceptCounters.emplace_back(i);
	}

	CalculateCoverageInfo(
		instances_original, rulesSoFar, answerClass,
		attributeCounters, conceptCounters);

	// find the best coverage
	struct SubRuleInfo
	{
		size_t AttIndex = 0;
		std::string ClassName = "";
		int Nominator = 0;
		int Denominator = 0;
		double Coverness = 0;
	};

	SubRuleInfo best;
	for (size_t att = 0; att < _dataframe.GetAttributeCount(); ++att)
	{
		if (att == _answerIdx)
			continue;

		std::vector<std::string> classes = std::move(
			attributeCounters[att].GetClasses());
		for (size_t i = 0; i < classes.size(); ++i)
		{
			int nominator = attributeCounters[att].GetCount(classes[i]);
			int denominator = conceptCounters[att].GetCount(classes[i]);
			double coverness = double(nominator) / denominator;

			bool updateBest = false;
			if (best.Coverness == coverness)
			{
				if (best.Nominator < nominator)
					updateBest = true;
			}
			else if (best.Coverness < coverness)
			{
				updateBest = true;
			}

			if (updateBest)
			{
				best.AttIndex = att;
				best.ClassName = classes[i];
				best.Nominator = nominator;
				best.Denominator = denominator;
				best.Coverness = coverness;
			}
			if (_o)
			{
				*_o << tabs
					<< _dataframe.GetAttributeName(att) 
					<< " = " << classes[i] << ", then " << answerClass
					<< " (" << nominator << "/" << denominator << ")"
					<< std::endl;
			}
		}
	}

	rulesSoFar.emplace_back(best.AttIndex, best.ClassName);

	if (_o)
	{
		(*_o).setf(std::ios::fixed);
		(*_o).precision(2);
		(*_o) << std::endl;
		(*_o) << tabs << "Choose : " 
			<< _dataframe.GetAttributeName(best.AttIndex)
			<< " = " << best.ClassName
			<< "( Highest Coverage : " << 
			best.Nominator << "/" << best.Denominator
			<< " = " << double(best.Nominator) / best.Denominator << ")"
			<< std::endl << std::endl;
	}

	if (best.Nominator == best.Denominator)
	{
		if(_o)
		{
			*_o << std::endl;
			*_o << "-- RULE " << ruleNum << " : ";
			for (size_t i = 0; i < rulesSoFar.size(); ++i)
			{
				if (i > 0)
					*_o << " and ";

				*_o << _dataframe.GetAttributeName(rulesSoFar[i].att)
					<< " = " << rulesSoFar[i].classValue;
			}
			*_o << ", then " << answerClass
				<< "( " << best.Nominator << "/"
				<< totalAnswerClassCount << " Covered )" 
				<< std::endl << std::endl;
		}

		instances_original = std::move(
			Filter(instances_original, SubruleExclusiveFunc(rulesSoFar)));
		return false;
	}
	
	return MakeRule(
		instances_original, answerClass, rulesSoFar, tabs + "\t", ruleNum);
}

void CoverageRule::Build()
{
	std::vector<Instance*> originalInstances = _dataframe.GetInstances();
	InstanceCategorizer counter(_answerIdx);
	for (auto& instance : originalInstances)
		counter.Add(instance);

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