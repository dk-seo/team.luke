#include "DecisionTreeTest.h"
#include "../ML/Dataframe/Dataframe.h"
#include "../ML/Classification/DecisionTree.h"
#include "../ML/Classification/MultiIntegralDiscretizer.h"
#include <iostream>
#include <cassert>

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
}

static AttributeType::Enum attributeTypes[WineAttribute::COUNT] = {
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Numeric,
	AttributeType::Nominal
};

class TreePrinter : public IDTVisitor
{
public:
	TreePrinter(std::ostream& o) : _o(o) {}
	virtual bool Visit(DecisionTree::Node* node)
	{
		_o << node->_attributeName;
		{
			_o << " (";
			bool notFirst = false;
			for (auto it : node->_childrenCountByConcept)
			{
				if (notFirst)		_o << ", ";
				else				notFirst = true;
				_o << it.first << "=" << it.second;
			}
			_o << ")" << std::endl;
		}

		if (node->_children.empty())
			return false;


		_tabs += "\t\t";
		auto& cutpoints = node->_discretizer->GetCutPoints();
		auto& itCutpoint = cutpoints.begin();
		auto& rightBeforeEnd = std::prev(node->_children.end());
		for (auto child = node->_children.begin(); child != rightBeforeEnd;
			++child)
		{
			_o << _tabs << "[ < " << *(itCutpoint++) << " ]";
			child->second->Walk(this, true);
		}
		for (auto child = rightBeforeEnd; child != node->_children.end(); ++child)
		{
			_o << _tabs << "[ > " << cutpoints.back() << " ]";
			child->second->Walk(this, true);
		}

		_tabs.pop_back();
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

	std::vector<size_t> selectiveAttributes = { 0, 1, 3, 4, 6, 8, 9, 12 };

	Dataframe wines;
	if (!wines.BuildFromCsv("Data/wine_both_red_n_white.csv", true, selectiveAttributes))
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
	// @hanstar17 todo:
	// balance the wine dataset such that # instances of red wine/white wine
	// are the same or similar. USE SAMPLING

	// build decision tree with discretized attributes
	DecisionTree decisiontree(wines, WineAttribute::wine_type);

	std::ofstream out("out/tree.txt", std::ios::out);
	decisiontree.SetDebugOutput(&out);
	decisiontree.Build();

	TreePrinter printer(std::cout);
	decisiontree.Walk(&printer, true);
}