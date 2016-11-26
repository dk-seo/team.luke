#define _CRT_SECURE_NO_WARNINGS
#include "Q3Window.h"
#include "UI\Imgui\imgui.h"
#include "UI\Imgui\imgui_impl_dx11.h"
#include "ML\Classification\DecisionTree.h"
#include "ML\Classification\MultiIntegralDiscretizer.h"
#include "UI\UserInterface.h"
#include "FileIO\csvparser.h"
#include "FileIO\FileSystem.h"
#include "ML\Utilities\Statistics.h"
#include <random>

namespace WineAttribute
{
	enum Enum
	{
		BEGIN,
		fixed_acidity_ = BEGIN,
		volatile_acidity,
		residual_sugar,
		chlorides,
		total_sulfur_dioxide,
		pH,
		sulphates,
		wine_type,
		COUNT
	};

	static const char* Names[] =
	{
		"fixed acidity",
		"volatile acidity",
		"residual sugar",
		"chlorides",
		"total sulfur dioxide",
		"pH",
		"sulphates",
		"wine type",
	};

	static AttributeType::Enum Types[] = {
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Numeric,
		AttributeType::Nominal
	};
}

static ImVec4 ImGui_RedWineColor(1, 0.3f, 0.3f, 1);
static ImVec4 ImGui_WhiteWineColor(0.7f, 0.7f, 1, 1);

class ImguiTreeVisitor : public IDTVisitor
{
public:
	ImguiTreeVisitor() {}

	virtual bool Visit(DecisionTree* tree)
	{
		tree->GetRoot()->_attributePrefix = "Tree Root";
		return true;
	}

	virtual bool Visit(DecisionTree::Node* node)
	{
		std::string nodename = node->_attributePrefix;
		auto nodeOpen = ImGui::TreeNode(nodename.c_str());

		std::string instanceInfo = " (";
		bool notFirst = false;
		for (auto it : node->_childrenCountByConcept)
		{
			if (notFirst)	instanceInfo += ", ";
			else			notFirst = true;

			instanceInfo += it.first + "=" + std::to_string(it.second);
		}
		instanceInfo += ")";

		ImGui::SameLine(0, 3);
		if (node->_children.empty())
		{
			if (node->_conceptClass == "red")
			{
				ImGui::TextColored(
					ImGui_RedWineColor, instanceInfo.c_str());
			}
			else
			{
				ImGui::TextColored(
					ImGui_WhiteWineColor, instanceInfo.c_str());
			}
		}
		else
		{
			ImGui::TextColored(
				ImVec4(0.0f, 0.5f, 0.0f, 1), instanceInfo.c_str());
		}

		if (nodeOpen && !node->_children.empty())
		{
			auto& cutpoints = node->_discretizer->GetCutPoints();
			auto& itCutpoint = cutpoints.begin();
			for (auto child = node->_children.begin();
				child != node->_children.end(); ++child)
			{
				child->second->_attributePrefix = "[ ";

				if (child != node->_children.begin())
				{
					child->second->_attributePrefix +=
						std::to_string(*(itCutpoint++)) + " < ";
				}

				child->second->_attributePrefix +=
					node->_attributeName;

				if (child != std::prev(node->_children.end()))
				{
					child->second->_attributePrefix +=
						" <= " + std::to_string(*itCutpoint);
				}

				child->second->_attributePrefix += " ]";
				child->second->Walk(this, true);
			}
		}
		if (nodeOpen)
			ImGui::TreePop();

		return false;
	}
};

static std::vector<size_t> selectedFeatures = { 0, 1, 3, 4, 6, 8, 9, 12 };

Dataframe Q3Window::LoadDataframe(
	const std::string& filename, const std::vector<size_t>& selectedFeatures)
{
	Dataframe wines;
	if (wines.BuildFromCsv(filename, true, selectedFeatures))
	{
		for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
			wines.SetAttributeType(i, WineAttribute::Types[i]);
	}

	return std::move(wines);
}

Q3Window::Q3Window()
	: _active(false)
	, _trainingsetFilename("wine_both_red_n_white.csv")
	, _testType(0)
	, _outputGeneratedNoticeTickCount(0)
{
}

Q3Window::~Q3Window()
{
}

void Q3Window::ImguiBuildDecisionTree()
{
	ImGui::TextWrapped(
		"Wine Type Discriminator distinguishes whether wine is Red"
		"or White with given constituents with Decision Tree.");
	ImGui::Separator();

	ImGui::Text(GetTrainingSetFilename().c_str());
	ImGui::SameLine(0, 10);
	if (ImGui::Button("Build Decision Tree"))
	{
		if (!_dataframe)
		{
			LoadDataframe();
		}

		if (_dataframe)
		{
			_decisionTree.reset(
				new DecisionTree(*_dataframe, WineAttribute::wine_type));

			_decisionTree->Build();
		}
	}
}

void Q3Window::ImguiShowTree()
{
	if (_decisionTree)
	{
		if (ImGui::CollapsingHeader("Show Tree"))
		{
			ImguiTreeVisitor visitor;
			_decisionTree->Walk(&visitor, true);
		}
	}
}


void Q3Window::ImguiTestFile()
{
	std::vector<std::string> files =
		std::move(GetFilesNameFromDirectory("UserData"));

	static int currIndex = int(files.size()) - 1;
	ImGui::Combo("Csv File", &currIndex, StringItemsGetter,
		&files, int(files.size()));

	if (size_t(currIndex) < files.size())
	{
		if (ImGui::Button("Test Data"))
		{
			Dataframe testdata = std::move(
				LoadDataframe("UserData/is_red_or_white.csv", selectedFeatures));

			for (size_t i = 0; i < testdata.GetInstanceCount(); ++i)
			{
				auto& inst = testdata.GetInstance(i);
				std::string result = _decisionTree->Classify(&inst);
				inst.GetAttribute(WineAttribute::wine_type).Set(result);
			}

			std::ofstream answerfile(
				"Out/answer_to_is_red_or_white.csv", std::ios::out);
			_outputGeneratedNotice =
				"Out/answer_to_is_red_or_white.csv generated!";
			_outputGeneratedNoticeTickCount = 100;
			testdata.ToCsv(answerfile);
		}
	}
	if (!_outputGeneratedNotice.empty())
		ImGui::Text(_outputGeneratedNotice.c_str());
}

void Q3Window::RandomizeInputInstance()
{
	static std::default_random_engine generator;
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);

	for (int i = 0; i < WineAttribute::COUNT; ++i)
	{
		if (WineAttribute::Types[i] == AttributeType::Numeric)
		{
			_inputInstance.GetAttribute(i).Set(std::to_string(_mins[i] +
				double(distribution(generator)) *  (_maxes[i] - _mins[i]))
			);
		}
	}
}

void Q3Window::ImguiTestInput()
{
	ImGui::SameLine();
	if (ImGui::Button("Randomize"))
	{
		RandomizeInputInstance();
	}
	ImGui::Separator();

	for (int i = 0; i < WineAttribute::COUNT; ++i)
	{
		if (WineAttribute::Types[i] == AttributeType::Numeric)
		{
			float v = _inputInstance.GetAttribute(i).AsDouble();
			ImGui::DragFloat(WineAttribute::Names[i], 
				&v, _mins[i], _maxes[i]);

			_inputInstance.GetAttribute(i).Set(std::to_string(double(v)));
		}
	}

	ImGui::Separator();
	auto type = _decisionTree->Classify(&_inputInstance);
	if (type == "red")
	{
		ImGui::TextColored(ImGui_RedWineColor,
			_decisionTree->Classify(&_inputInstance).c_str());
	}
	else
	{
		ImGui::TextColored(ImGui_WhiteWineColor,
			_decisionTree->Classify(&_inputInstance).c_str());
	}
}

void Q3Window::ImguiTest()
{
	if (_decisionTree)
	{
		if (ImGui::CollapsingHeader("Test"))
		{
			ImGui::RadioButton("With File", &_testType, 0); ImGui::SameLine();
			ImGui::RadioButton("With Input", &_testType, 1);

			switch (_testType)
			{
			case 0:
				ImguiTestFile();
				break;
			case 1:
				ImguiTestInput();
				break;
			}
		}
	}
}

void Q3Window::Update()
{
	if (_outputGeneratedNoticeTickCount > 0)
		--_outputGeneratedNoticeTickCount;

	if (!IsActive())
		return;

	ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_Once);
	if(ImGui::Begin("Wind Type Taster", &_active, ImVec2(0, 0)))
	{
		ImguiBuildDecisionTree();

		ImguiTest();

		ImguiShowTree();

		ImGui::End();
	}

	if (_errorWindowMessage.length() > 0)
	{
		bool open = true;
		if(ImGui::Begin("Error", &open, ImVec2(0, 0)))
		{
			if (!open) 
				_errorWindowMessage = "";

			ImGui::Text(_errorWindowMessage.data());
			ImGui::End();
		}
	}
}

bool Q3Window::IsValidTrainingSet()
{
	CsvParser *csvParser = CsvParser_new(
		GetTrainingSetFilename().c_str(), ",", int(true));
	if (!csvParser)
	{
		_errorWindowMessage =
			"Invalid training set.\n"
			"The file" + GetTrainingSetFilename() +" doesn't have heaer row.";
		return false;
	}

	CsvRow* header = CsvParser_getHeader(csvParser);
	if (!header)
	{
		_errorWindowMessage =
			"Invalid training set.\n"
			"The file doesn't have heaer row.";
		CsvParser_destroy(csvParser);
		return false;
	}

	const char originalAttributeNames[][100] = {
		"fixed acidity",
		"volatile acidity",
		"citric acid",
		"residual sugar",
		"chlorides",
		"free sulfur dioxide",
		"total sulfur dioxide",
		"density",
		"pH",
		"sulphates",
		"alcohol",
		"quality",
		"wine type"
	};

	bool invalidheader = false;
	int attributeCount = CsvParser_getNumFields(header);
	if (attributeCount != 13)
		invalidheader = true;

	if (!invalidheader)
	{
		const char **parsedHeader = CsvParser_getFields(header);
		int farthestSameIdx = 0;
		for (; farthestSameIdx < 13; ++farthestSameIdx)
		{
			if (strcmp(parsedHeader[farthestSameIdx],
				originalAttributeNames[farthestSameIdx]) != 0)
			{
				break;
			}
		}
		if (farthestSameIdx < 13)
			invalidheader = true;
	}

	CsvParser_destroy(csvParser);

	if (invalidheader)
	{
		_errorWindowMessage =
			"Invalid training set.\n"
			"The file has to have the following 13 Attributes:\n";

		for (int i = 0; i < 13; ++i)
		{
			_errorWindowMessage +=
				std::string(originalAttributeNames[i]) + "\n";
		}

		return false;
	}
	return true;
}

bool Q3Window::LoadDataframe()
{
	if (!IsValidTrainingSet())
		return false;

	_dataframe.reset(new Dataframe);
	if (!_dataframe->BuildFromCsv(
		GetTrainingSetFilename().c_str(), true, selectedFeatures))
	{
		_errorWindowMessage = "Building from training dataset file failed!";
		return false;
	}
	
	_mins.resize(WineAttribute::COUNT);
	_maxes.resize(WineAttribute::COUNT);
	std::vector<Instance*> instances = std::move(_dataframe->GetInstances());
	if (instances.empty())
	{
		_errorWindowMessage = "Instances doesn't exist in training data set!";
		return false;
	}

	for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
	{
		if (WineAttribute::Types[i] == AttributeType::Numeric)
		{
			_mins[i] = FindMin(instances, i);
			_maxes[i] = FindMax(instances, i);
		}
		_dataframe->SetAttributeType(i, WineAttribute::Types[i]);
	}

	_inputInstance = *instances[0];

	return true;
}
