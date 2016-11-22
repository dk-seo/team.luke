#include "Gain.h"
#include <vector>
#include <string>
#include <fstream>

double Entropy(const std::vector<double>& values)
{
	double entropy = 0.0;
	for each(auto value in values)
		entropy -= value*log2(value);

	return entropy;
}

double Info(std::ostream* o,
	const std::string& name, 
	const std::string& tabs, 
	const std::vector<int>& items)
{
	int sum = 0;
	for (unsigned i = 0; i < items.size(); ++i)
		sum += items[i];


	if (o)
	{
		*o << tabs << name << " Info([";

		for (int i = 0; i < int(items.size()); ++i)
		{
			if (i > 0)
				*o << ", ";
			*o << items[i];
		}

		*o << "]) = ";
	}


	std::vector<double> values;
	for each(auto item in items)
	{
		std::string term = std::to_string(item) + "/" + std::to_string(sum);
		if(o)
			*o << " - (" << term << ")log_2(" << term << ")";
		values.push_back(double(item) / sum);
	}

	double entropy = Entropy(values);
	if(o)
		*o << " = " << entropy << std::endl;
	
	return entropy;
}

typedef std::vector<int> Vector;

int Sum(const std::vector<int>& vec)
{
	int sum = 0;
	for (auto& v : vec)
		sum += v;
	return sum;
}

struct Fraction
{
	Fraction(int n, int d) : nominator(n), denominator(d) {}
	int nominator;
	int denominator;
	std::string Desc() const {
		return std::string("(") + std::to_string(nominator) + "/" + std::to_string(denominator) + ")";
	}
	double Eval() const {
		return double(nominator) / denominator;
	}
};

double Gain(
	std::ostream* o,
	const std::string& name,
	const std::vector<int>& vector,
	const std::vector<std::string>& childrenNames,
	const std::vector<std::vector<int>>& children,
	bool printMetadata,
	std::string tabs)
{
	if(o)
		*o << tabs << "------" << std::endl;
	double parentInfo = Info(o, name, tabs, vector);
	int totalSum = Sum(vector);

	std::vector<Fraction> coefficients;
	std::vector<double> infos;
	for (size_t i = 0; i < children.size(); ++i)
	{
		double info = Info(o, i < childrenNames.size() ? childrenNames[i] : "", tabs, children[i]);
		infos.emplace_back(info);
		coefficients.emplace_back(Sum(children[i]), totalSum);
	}

	// print gain
	if(o)
		*o << tabs << "gain = " << parentInfo;

	double gain = parentInfo;
	for (size_t i = 0; i < infos.size(); ++i)
	{
		double info = infos[i];
		if (o)
		{
			*o << " - ";
			*o << coefficients[i].Desc() << " * " << info;
		}
		gain -= coefficients[i].Eval() * infos[i];
	}
	if(o)
		*o << " = " << gain << std::endl;
	return gain;
}
