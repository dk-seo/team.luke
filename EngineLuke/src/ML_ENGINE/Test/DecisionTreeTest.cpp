#include "DecisionTreeTest.h"
#include "../Dataframe/Dataframe.h"
#include "../Classification/DecisionTree.h"
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

class TreePrinter : public IDTVisitor
{
public:
	TreePrinter(std::ostream& o) : _o(o) {}
	virtual bool Visit(DecisionTree::Node* node)
	{
		_o << _tabs <<
			node->_attributeName;
		if (node->_children.empty())
			_o << "==>" << node->_conceptClass;
		else
			
		_o << std::endl;

		_tabs += "\t";
		node->Walk(this, false);
		_tabs.pop_back();
		return false;
	}

private:
	std::ostream& _o;
	std::string _tabs;
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

Dataframe OpenMergedWineDataframe()
{
	// build red/white data frames and merge them with wine type column added
	//Dataframe wines;
	//if (!wines.BuildFromCsv(
	//	"Data/winequality-red_Output.csv", true))
	//{
	//	std::cout << "loading red-wine data failed!" << std::endl;
	//	return;
	//}
	//AddWineAttribute(
	//	wines, "wine type", "red", attributeTypes[WineAttribute::wine_type]);

	//{
	//	Dataframe whitewines;
	//	if (!whitewines.BuildFromCsv(
	//		"Data/winequality-white_Output.csv", true))
	//	{
	//		std::cout << "loading red-wine data failed!" << std::endl;
	//		return;
	//	}
	//	AddWineAttribute(
	//		whitewines, "wine type", "white",
	//		attributeTypes[WineAttribute::wine_type]);

	//	wines.Merge(whitewines);
	// wines.ToCsv(mergedCsv);
	//}

	Dataframe wines;
	if (!wines.BuildFromCsv("wine_both_red_n_white.csv", true))
	{
		std::cout << "no file wine_both_red_n_white.csv" << std::endl;
	}
	else
	{
		for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
			wines.SetAttributeType(i, attributeTypes[i]);
	}

	return std::move(wines);
}


void DecisionTreeTest_Wines()
{
	Dataframe wines = std::move(OpenMergedWineDataframe());

	// build decision tree with discretized attributes
	DecisionTree decisiontree(wines, WineAttribute::wine_type);

	std::ofstream out("out/tree.txt", std::ios::out);
	decisiontree.SetDebugOutput(&out);
	decisiontree.Build();

	TreePrinter printer(std::cout);
	decisiontree.Walk(&printer, true);
}