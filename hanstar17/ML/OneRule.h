#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Instance;

class OneRule
{
	// for print
	friend std::ostream& operator<< (std::ostream& o, OneRule& rhs);

	// Holds statstics for an attribute type
	class AttributeStastics
	{
		// holds frequency of each class of a concept
		typedef std::unordered_map<std::string, int> ConceptFrequencies;
		// container that holds frequencies of concepts for each class of attribute
		typedef std::unordered_map<std::string, ConceptFrequencies> AttributeConceptMap;

	public:
		// Rule for a class of an attribute
		struct ClassRule
		{
			std::string Attribute;
			std::string Concept;
			int ErrorCount;
		};

		// Rule for an attribute
		struct Rule
		{
			int ErrorCount;
			std::vector<ClassRule> ClassRules;
		};

	public:
		std::string GetName() const { return _name; }
		void SetName(const std::string& name);

		// add case with attribute and matching answer
		void AddCase(const std::string& attribute, const std::string& answer);

		// evaluate instance so far and extract a rule
		Rule Evaluate() const;
		
	private:
		AttributeConceptMap _attributeConceptMap;
		std::string _name;
	};

public:
	OneRule();

	// Add attribute
	void AddAttribute(const std::string& attributeName);
	
	// Feed with instance (the last attribute is considered as concept -- aka answer)
	void Feed(const Instance& instance); 

	// Induce concept (an answer for the last attribute) using 1R 
	std::string Induce(const Instance& instance);

	
private:
	// Process learning by far and deduce one rule based on feeds by far.
	void Digest();

private:
	std::vector<AttributeStastics> _attStatistics; // statistics per attribute
	std::vector<AttributeStastics::Rule> _attEvaluations; // evaluations for debug printing
	int _oneRuleIdx; // index of attribute that's used as one rule
	bool _lazydigest; // flag for lazy digest
};

// print helper
std::ostream& operator<< (std::ostream& o, OneRule& rhs);