#include "OneRule.h"

OneRule::OneRule(void)
{

}
OneRule::~OneRule(void)
{

}

RuleClass OneRule::OneRAlgorithm(const DATASET* dataset)
{
	
	std::vector<RuleClass> ruleclassList;        // Container for Rule class List
	size_t attr_size = dataset->size();          // Number of attribute for given data set
	size_t data_size = dataset->front().size();  // Total number of data from data set

	//Insert attribute name at the first point of each vector 
	for (int i = 0; i < attr_size - 1; ++i)
	{
		RuleClass ruleclass; 
		ruleclass.attr = (*dataset)[i][0];
		ruleclassList.push_back(ruleclass);
	}

	for (int data_i = 1; data_i < data_size; ++data_i)
	{
		for (int attr_i = 0; attr_i < attr_size - 1; ++attr_i)
		{
			//Insert new rule or if it already exist, then increase the count
			CheckRuleIn(&ruleclassList[attr_i], (*dataset)[attr_i][data_i], (*dataset)[attr_size - 1][data_i]);
		}
	}

	int totalerrorMin = 0;
	// Calculate error and find result class
	for (int i = 0; i < attr_size - 1; ++i)
	{
		int sum = 0;
		size_t rulesize = ruleclassList[i].rules.size();
		for (int j = 0; j < rulesize; ++j)
		{
			int max = 0;
			size_t resultsize = ruleclassList[i].rules[j].fin.size();
			
			// Find index of max appearence rule 
			for (int k = 0; k < resultsize; ++k)
			{
				if (ruleclassList[i].rules[j].fin[max].appearence < ruleclassList[i].rules[j].fin[k].appearence)
					max = k;
			}
			ruleclassList[i].rules[j].maxindex = max;
			sum += ruleclassList[i].rules[j].fin[max].appearence;
		}

		// Calculate total error, which subtract max apperence from total num
		ruleclassList[i].totalerror = int(data_size) - 1 - sum;

		// Find less error class, and remember that index of class
		if (ruleclassList[totalerrorMin].totalerror > ruleclassList[i].totalerror)
			totalerrorMin = i;
	}

	return ruleclassList[totalerrorMin];
}

void OneRule::CheckRuleIn(RuleClass* list, std::string attdata, std::string result)
{
	size_t size = list->rules.size();

	// If this is first rule, then just insert into the vector
	if (size == 0)
	{
		Rule newrule;
		newrule.attr_data = attdata;
		Result temp;
		temp.result = result;
		newrule.fin.push_back(temp);
		list->rules.push_back(newrule);
		return;
	}

	bool check = true;
	for (int i = 0; i < size; ++i)
	{
		// Grab if left hand attribute is same with new one
		if (list->rules[i].attr_data == attdata)// && list->rules[i].result == result)
		{
			for (int j = 0; j < list->rules[i].fin.size(); ++j)
			{
				// Grab if right hand attribute is same with new one
				if (result == list->rules[i].fin[j].result)
				{
					// This means that the rule is already in the list, just increase the count
					++list->rules[i].fin[j].appearence;
					return;
				}
			}
			// This means that there is same left hand attribute,
			// But there is no exactly same rule 
			// Insert new rule
			if (check)
			{
				Result temp;
				temp.result = result;
				list->rules[i].fin.push_back(temp);
				return;
			}
		}
	}

	// This means also, adding new rule
	// No same attribute of both left or right side
	Rule newrule;
	newrule.attr_data = attdata;
	Result temp;
	temp.result = result;
	newrule.fin.push_back(temp);
	list->rules.push_back(newrule);
}

void OneRule::Classify(DATASET* newinput, RuleClass result_rule)
{
	// Using the result rule class, ans classify new input
	for (int i = 0; (*newinput).size(); ++i)
	{
		if ((*newinput)[i][0] == result_rule.attr)
		{
			for (int dataindex = 1; dataindex < (*newinput)[i].size(); ++dataindex)
			{
				for (int j = 0; j < result_rule.rules.size(); ++j)
				{
					if (result_rule.rules[j].attr_data == (*newinput)[i][dataindex])
					{
						// Fallow that result class, update result attribute of new input
						int maxindex = result_rule.rules[j].maxindex;
						(*newinput)[(*newinput).size() - 1][dataindex] = result_rule.rules[j].fin[maxindex].result;
					}
				}
			}
			break;
		}
	}
}

