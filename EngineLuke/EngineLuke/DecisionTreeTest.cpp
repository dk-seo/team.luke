#include "DecisionTreeTest.h"
#include "Dataframe.h"
#include "DecisionTree.h"
#include <iostream>
#include <cassert>

namespace WineAttribute
{
	enum Enum
	{
		fixed_acidity_,
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

static void AddWineAttribute(
	Dataframe& dataframe, 
	const std::string& attributeName,
	const std::string& desiredClass)
{
	dataframe.AddAttribute(attributeName);
	for (auto instance : dataframe.GetInstances())
	{
		instance->AddAttribute(desiredClass);
		assert(instance->GetAttributeCount() == dataframe.GetAttributeCount());
	}
}


static void DiscritizeDataframe(
	Dataframe& dataframe,
	std::vector<size_t> attToDiscretize,
	size_t answerIdx)
{

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
	AddWineAttribute(wines, "wine type", "red");

	{
		Dataframe whitewines;
		if (!whitewines.BuildFromCsv(
			"Data/winequality-white_Output.csv", true))
		{
			std::cout << "loading red-wine data failed!" << std::endl;
			return;
		}
		AddWineAttribute(whitewines, "wine type", "white");

		wines.Merge(whitewines);
	}

	std::ofstream of("wine_both_red_n_white.csv", std::ios::out);
	wines.ToCsv(of);

	// do feature selection (if time allowed)
	// ...

	// discritize each attribute
	std::vector<size_t> attToDiscretize =
	{
		WineAttribute::fixed_acidity_,
		WineAttribute::volatile_acidity,
		WineAttribute::citric_acid,
		WineAttribute::residual_sugar,
		WineAttribute::chlorides,
		WineAttribute::free_sulfur_dioxide,
		WineAttribute::total_sulfur_dioxide,
		WineAttribute::density,
		WineAttribute::pH,
		WineAttribute::sulphates,
		WineAttribute::alcohol
	};
	DiscritizeDataframe(wines, attToDiscretize, WineAttribute::wine_type);

	// build decision tree with discritized attributes
	DecisionTree decisiontree(wines, WineAttribute::wine_type);
	decisiontree.Build();

	// test
	// ...
}