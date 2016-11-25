#include "Similarity.h"
#include "../Dataframe/Dataframe.h"
#include <algorithm>

double CalculateMean(
	const std::vector<Instance*>& instances, size_t attributeIdx)
{
	if (instances.size() == 0)
		return 0.0;

	double mean = 0.0f;
	for (auto instance : instances)
		mean += double(instance->GetAttribute(attributeIdx).AsDouble());
	mean /= double(instances.size());
	return mean;
}

double CalculateMedian(
	std::vector<Instance*> instances, size_t attributeIdx)
{

	// sort instances by attribute value
	std::sort(instances.begin(), instances.end(), 
		[&attributeIdx](const Instance* lhs, const Instance* rhs) {
			return lhs->GetAttribute(attributeIdx).AsDouble()
				< rhs->GetAttribute(attributeIdx).AsDouble();
		}
	);

	// calculate median
	size_t middle = instances.size() / 2;
	double median;
	if (instances.size() % 2)
	{
		// if there are an even number of instances, median is the middlemost value
		median = instances[middle]->GetAttribute(attributeIdx).AsDouble();
	}
	else
	{
		// if there are an odd number of instances, median is the average of the two middlemost values
		median = (instances[middle]->GetAttribute(attributeIdx).AsDouble()
			+ instances[middle - 1]->GetAttribute(attributeIdx).AsDouble())
			* 0.5;
	}
	return median;
}

std::vector<double> CalculateMode(
	const std::vector<Instance*>& instances, size_t attributeIdx)
{
	// find modes
	std::unordered_map<double, int> frequencies;
	int highestFrequency = std::numeric_limits<int>::min();
	for (unsigned i = 0; i < instances.size(); ++i)
	{
		auto frequency = frequencies.find(
			instances[i]->GetAttribute(attributeIdx).AsDouble());

		if (frequency == frequencies.end())
			frequencies.emplace(
				instances[i]->GetAttribute(attributeIdx).AsDouble(), 1);
		else if (++frequency->second > highestFrequency)
			highestFrequency = frequency->second;
	}

	// frequency 1 is not considered as mode, so exclude the case
	std::vector<double> modes;
	if (highestFrequency > 1)
	{
		for (auto frequency : frequencies)
		{
			if (frequency.second == highestFrequency)
				modes.emplace_back(frequency.first);
		}
	}

	return std::move(modes);
}