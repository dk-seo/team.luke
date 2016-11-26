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

std::vector<size_t> s_selectedFeatures = { 0, 1, 3, 4, 6, 8, 9, 12 };

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
			_o << ")";
		}

		if (node->_children.empty())
		{
			_o << "-->" << node->_conceptClass << std::endl;
			return false;
		}
			
		_o << std::endl;

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

Dataframe OpenWineData(const std::string& filename, const std::vector<size_t>& selectedFeatures = {})
{
	Dataframe wines;
	if (!wines.BuildFromCsv(filename, true, selectedFeatures))
	{
		std::cout << "no file exists : " << filename << std::endl;
	}
	else
	{
		for (size_t i = WineAttribute::BEGIN; i < WineAttribute::COUNT; ++i)
			wines.SetAttributeType(i, WineAttribute::Types[i]);
	}

	return std::move(wines);
}


void CreateMergedData(const std::string& mergedFilename)
{
	Dataframe wines = std::move(OpenWineData("Data/winequality-red_Output.csv"));
	AddWineAttribute(wines, 
		WineAttribute::Names[WineAttribute::wine_type], 
		"red", 
		AttributeType::Nominal);

	Dataframe whitewines =
		std::move(OpenWineData("Data/winequality-white_Output.csv"));
	AddWineAttribute(whitewines, 
		WineAttribute::Names[WineAttribute::wine_type], 
		"white", 
		AttributeType::Nominal);

	if (!wines.Merge(whitewines))
	{
		std::cout << "merge failed!" << std::endl;
		return;
	}

	std::ofstream o(mergedFilename, std::ios::out);
	wines.ToCsv(o);
}


std::string ReduceDataByFeatureSelection(const std::string& filename)
{
	Dataframe d = std::move(OpenWineData(filename, s_selectedFeatures));
	std::string reducedFilename = 
		filename.substr(0, filename.find_last_of('.'))
		+ "_feature_selection.csv";

	std::ofstream o(reducedFilename, std::ios::out);
	d.ToCsv(o);
	return std::move(reducedFilename);
}

void DecisionTreeTest_Wines(std::ostream& treeout)
{
	std::string filename = "Data/wine_both_red_n_white.csv";
	//CreateMergedData(filename);
	//ReduceDataByFeatureSelection(filename);

	Dataframe wines = std::move(OpenWineData(filename, s_selectedFeatures));

	// @hanstar17 todo:
	// balance the wine dataset such that # instances of red wine/white wine
	// are the same or similar. USE SAMPLING

	// build decision tree with discretized attributes
	DecisionTree decisiontree(wines, WineAttribute::wine_type);

	decisiontree.Build();

	TreePrinter printer(treeout);
	decisiontree.Walk(&printer, true);

	Dataframe testdata = std::move(
			OpenWineData("UserData/is_red_or_white.csv", s_selectedFeatures));

	for (size_t i = 0; i < testdata.GetInstanceCount(); ++i)
	{
		auto& inst = testdata.GetInstance(i);
		std::string result = decisiontree.Classify(&inst);
		inst.GetAttribute(WineAttribute::wine_type).Set(result);
	}

	std::ofstream answerfile(
		"Out/answer_to_is_red_or_white.csv", std::ios::out);

	std::cout 
		<< "user input file \"UserData/is_red_or_white.csv\" processed." 
		<< std::endl;
	std::cout
		<< "Check out the file \"/Out/answer_to_is_red_or_white.csv\""
		<< std::endl << std::endl;

	testdata.ToCsv(answerfile);
}
