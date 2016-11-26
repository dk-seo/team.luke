#pragma once
#include <memory>
#include <string>

class DecisionTree;
class Dataframe;

class Q3Window
{
public:
	Q3Window();
	~Q3Window();

	bool IsActive() const { return _active; }
	void SetActive(bool active) { _active = active; }

	void Update();

private:
	bool LoadDataframe();
	bool IsValidTrainingSet();
	std::string GetTrainingSetFilename() { 
		return "Data/wine_both_red_n_white.csv"; }

private:
	bool _active;
	std::unique_ptr<DecisionTree> _decisionTree;
	std::unique_ptr<Dataframe> _dataframe;
	std::string _errorWindowMessage;
	char _trainingsetFilename[255];

};
