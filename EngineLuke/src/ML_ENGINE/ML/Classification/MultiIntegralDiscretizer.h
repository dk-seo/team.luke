#pragma once
#include <vector>
class Instance;
class InstanceCategorizer;

class MultiIntegralDiscretizer
{
public:
	MultiIntegralDiscretizer(size_t attributeIdx, size_t answerIdx);
	~MultiIntegralDiscretizer();

	void Build(const std::vector<Instance*>& instances);

	std::string Discretize(const Instance* instance);

	const std::vector<double>& GetCutPoints() const { return _cutPoints; }

private:
	void Split(InstanceCategorizer& totalInstanceCategorizer, size_t depth);

	bool ShouldAccept(
		double gain,
		InstanceCategorizer& total,
		InstanceCategorizer& left,
		InstanceCategorizer& right);

private:
	size_t _attributeIdx;
	size_t _answerIdx;
	size_t _maxSplitDepth;
	std::vector<double> _cutPoints;
};

