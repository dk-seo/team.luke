#include "DecisionTreeTest.h"
#include "Dataframe.h"
#include "DecisionTree.h"
#include <iostream>
#include <cassert>

namespace WineAttribute
{
	enum Enum
	{
		BEGIN,
		fixed_acidity_ = BEGIN,
		volatile_acidity,
		citric_acid,
		residual_sugar,
		chlorides,
		free_sulfur_dioxide,
		total_sulfur_dioxide,
		density,
		pH,
		sulphates,
		alcohol,
		quality,
		wine_type,
		COUNT
	};
}

static AttributeType::Enum attributeTypes[WineAttribute::COUNT] = {
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Nominal, // quality
	AttributeType::Nominal // wine type
};

static void AddWineAttribute(
	Dataframe& dataframe, 
	const std::string& attributeName,
	const std::string& desiredClass,
	AttributeType::Enum attType)
{
	dataframe.AddAttribute(attributeName, attType);
	for (auto instance : dataframe.GetInstances())
	{
		instance->AddAttribute(desiredClass);
		assert(instance->GetAttributeCount() == dataframe.GetAttributeCount());
	}
}

void DecisionTreeTest_Wines()
{
	// build red/white data frames and merge them with wine type column added
	Dataframe wines;
	if (!wines.BuildFromCsv(
		"Data/winequality-red_Output.csv", true))
	{
		std::cout << "loading red-wine data failed!" << std::endl;
		return;
	}
	AddWineAttribute(
		wines, "wine type", "red", attributeTypes[WineAttribute::wine_type]);

	{
		Dataframe whitewines;
		if (!whitewines.BuildFromCsv(
			"Data/winequality-white_Output.csv", true))
		{
			std::cout << "loading red-wine data failed!" << std::endl;
			return;
		}
		AddWineAttribute(
			whitewines, "wine type", "white", 
			attributeTypes[WineAttribute::wine_type]);

		wines.Merge(whitewines);
	}



	std::ofstream mergedCsv("wine_both_red_n_white.csv", std::ios::out);
	wines.ToCsv(mergedCsv);

	for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
		wines.SetAttributeType(i, attributeTypes[i]);

	// do feature selection (if time allowed)

	// build decision tree with discretized attributes
	DecisionTree decisiontree(wines, WineAttribute::wine_type);
	decisiontree.Build();
}