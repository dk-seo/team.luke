#include "OneRule.h"
#include "../Dataframe/Dataframe.h"
#include <algorithm>

void OneRule::AttributeStastics::AddCase(const std::string& aCase, const std::string& answer)
{
	// get the concept frequencies of this case and increase count of matching concept
	ConceptFrequencies& conceptFrequencies = _attributeConceptMap[aCase];
	auto frequencyIter = conceptFrequencies.find(answer);
	if (frequencyIter != conceptFrequencies.end())
	{
		++frequencyIter->second;
	}
	else
	{
		conceptFrequencies.emplace(answer, 1);
	}
}

void OneRule::AttributeStastics::SetName(const std::string& name)
{
	_name = name;
}

OneRule::AttributeStastics::Rule OneRule::AttributeStastics::Evaluate() const
{
	Rule rule;
	rule.ErrorCount = 0;
	// for each class of this attribute, calculate frequencies of each of matching concepts
	for (auto conceptIter : _attributeConceptMap)
	{
		int totalFrequency = 0; 
		int maxFrequency = 0;
		std::string maxFrequencyConcept; 

		for (auto frequencyIter : conceptIter.second)
		{
			totalFrequency += frequencyIter.second;
			if (maxFrequency < frequencyIter.second)
			{
				maxFrequency = frequencyIter.second;
				maxFrequencyConcept = frequencyIter.first;
			}
		}

		ClassRule classRule;
		classRule.Attribute = conceptIter.first;
		classRule.Concept = maxFrequencyConcept;
		classRule.ErrorCount = totalFrequency - maxFrequency;
		rule.ErrorCount += classRule.ErrorCount;
		rule.ClassRules.emplace_back(std::move(classRule));
	}
	return std::move(rule);
}

OneRule::OneRule()
	: _lazydigest(false)
{
}

void OneRule::AddAttribute(const std::string& attributeName)
{
	AttributeStastics as;
	as.SetName(attributeName);
	_attStatistics.emplace_back(as);
}

void OneRule::Feed(const Instance& instance)
{
	const int attCount = instance.GetAttributeCount();
	const std::string& answer = instance.GetAttribute(attCount - 1).AsString();

	for (int i = 0; i < attCount - 1; ++i) // last attribute is omitted as it's concept
	{
		const std::string& attribute = instance.GetAttribute(i).AsString();
		_attStatistics[i].AddCase(attribute, answer);
	}

	_lazydigest = true;
}

void OneRule::Digest()
{
	_attEvaluations.clear();

	// find an attribute rule that has the least error
	// Also, saves each evaluated rule for debugging purpose.
	int attributeCount = (int)_attStatistics.size();
	_oneRuleIdx = 0;
	for (int i = 0; i < attributeCount - 1; ++i)
	{
		AttributeStastics::Rule rule = _attStatistics[i].Evaluate();
		_attEvaluations.push_back(rule);
		if (rule.ErrorCount < _attEvaluations[_oneRuleIdx].ErrorCount)
		{
			_oneRuleIdx = i;
		}
	}

	_lazydigest = false;
}

std::string OneRule::Induce(const Instance& instance)
{
	if (_lazydigest)
		Digest();

	const std::string& attribute = instance.GetAttribute(_oneRuleIdx).AsString();
	const AttributeStastics::Rule& oneRule = _attEvaluations[_oneRuleIdx];
	for (const auto& subRule : oneRule.ClassRules)
	{
		if (attribute == subRule.Attribute)
		{
			return subRule.Concept;
		}
	}

	// if non-existing class is tested, just print any concept value
	// because it can't be figured out with 1R in principle.
	return oneRule.ClassRules[0].Concept;
}

std::ostream& operator<< (std::ostream& o, OneRule& rhs)
{
	if (rhs._lazydigest)
		rhs.Digest();

	for (unsigned i = 0; i < rhs._attEvaluations.size(); ++i)
	{
		if (i == rhs._oneRuleIdx)
			o << "*";
		o << "Rule for: " << rhs._attStatistics[i].GetName()
			<< "(TotalError=" << rhs._attEvaluations[i].ErrorCount << ")"
			<< std::endl;

		for (auto& subRule : rhs._attEvaluations[i].ClassRules)
		{
			o << "\t"
				<< subRule.Attribute
				<< " -> "
				<< subRule.Concept
				<< " (Error="
				<< subRule.ErrorCount
				<< ")"
				<< std::endl;
		}
	}
	return o;
}
