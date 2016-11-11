#pragma once

#include "def.h"

class OneRule
{
public:
	OneRule(void);
	~OneRule(void);

	RuleClass OneRAlgorithm(const DATASET* dataset);
	void CheckRuleIn(RuleClass* list, std::string attdata, std::string result);
	void Classify(DATASET* newinput, RuleClass result_rule);
	

private:

};