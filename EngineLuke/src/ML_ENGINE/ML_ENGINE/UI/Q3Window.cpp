#define _CRT_SECURE_NO_WARNINGS
#include "Q3Window.h"
#include "UI\Imgui\imgui.h"
#include "UI\Imgui\imgui_impl_dx11.h"
#include "ML\Classification\DecisionTree.h"
#include "UI\UserInterface.h"
#include "FileIO\csvparser.h"
#include "FileIO\FileSystem.h"

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


Q3Window::Q3Window()
	:_active(false)
	,_trainingsetFilename("wine_both_red_n_white.csv")
{
}

Q3Window::~Q3Window()
{
}

void Q3Window::Update()
{
	if (!IsActive())
		return;

	ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_Once);
	ImGui::Begin("Answer for Q3", nullptr, ImVec2(0, 0));
	{
		ImGui::InputText(
			"Training Set", _trainingsetFilename, sizeof(_trainingsetFilename));

		if (ImGui::Button("Load Dataframe"))
		{
			LoadDataframe();
		}

		if (_dataframe)
		{
			if (ImGui::Button("Build Decision Tree"))
			{
				_decisionTree.reset(
					new DecisionTree(*_dataframe, WineAttribute::wine_type));
				_decisionTree->Build();
			}
		}
		
	}
	ImGui::End();

	if (_errorWindowMessage.length() > 0)
	{
		bool open = true;
		ImGui::Begin("Error", &open, ImVec2(0, 0));
		{
			if (!open)
				_errorWindowMessage = "";

			ImGui::Text(_errorWindowMessage.data());
		}
		ImGui::End();
	}
}

bool Q3Window::IsValidTrainingSet()
{
	CsvParser *csvParser = CsvParser_new(
		GetTrainingSetFilename().c_str(), ",", int(true));
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

	static std::vector<size_t> selectedFeatures = { 0, 1, 3, 4, 6, 8, 9, 12 };

	
	_dataframe.reset(new Dataframe);
	if (!_dataframe->BuildFromCsv(
		GetTrainingSetFilename().c_str(), true, selectedFeatures))
	{
		_errorWindowMessage = "Building from training dataset file failed!";
		return false;
	}
	
	for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
		_dataframe->SetAttributeType(i, WineAttribute::Types[i]);

	return true;
}
