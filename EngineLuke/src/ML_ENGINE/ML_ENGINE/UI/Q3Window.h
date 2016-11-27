#pragma once
#include <memory>
#include <string>
#include "ML\Dataframe\Dataframe.h"

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
	std::string GetTrainingSetFilename() const { 
		return "Data/wine_both_red_n_white.csv"; }

	Dataframe LoadDataframe(
		const std::string& filename, 
		const std::vector<size_t>& selectedFeatures = {});


	void ImguiBuildDecisionTree();
	void ImguiShowTree();
	void ImguiTest();

	void ImguiTestFile();
	void ImguiTestInput();


	void RandomizeInputInstance();
private:
	bool _active;
	std::unique_ptr<DecisionTree> _decisionTree;
	std::unique_ptr<Dataframe> _dataframe;
	std::string _errorWindowMessage;
	std::string _outputGeneratedNotice;
	std::vector<float> _mins;
	std::vector<float> _maxes;
	Instance _inputInstance;
	char _trainingsetFilename[255];
	size_t _outputGeneratedNoticeTickCount;
	int _testType;

};
