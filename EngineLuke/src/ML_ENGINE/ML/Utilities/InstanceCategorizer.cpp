#include "InstanceCategorizer.h"
#include "../Dataframe/Dataframe.h"
#include "../Classification/MultiIntegralDiscretizer.h"

InstanceCategorizer::InstanceCategorizer(
	size_t attIdx, MultiIntegralDiscretizer* discretizer)
	: _attributeIdx(attIdx)
	, _discretizer(discretizer)
{
}

InstanceCategorizer::~InstanceCategorizer()
{
}

InstanceCategorizer::InstanceCategorizer(InstanceCategorizer&& rhs)
{
	_attributeIdx = rhs._attributeIdx;
	_map = std::move(rhs._map);
	_discretizer = std::move(rhs._discretizer);
}

InstanceCategorizer& InstanceCategorizer::operator=(InstanceCategorizer&& rhs)
{
	_attributeIdx = rhs._attributeIdx;
	_map = std::move(rhs._map);
	_discretizer = std::move(rhs._discretizer);
	return *this;
}

void InstanceCategorizer::Add(Instance* instance)
{
	std::string className = instance->GetAttribute(_attributeIdx).AsString();
	if (_discretizer)
		_map[_discretizer->Discretize(instance)].emplace_back(instance);
	else
		_map[className].emplace_back(instance);
}

int InstanceCategorizer::GetCount()
{
	int totalcount = 0;
	for (auto& classtypes : _map)
		totalcount += int(classtypes.second.size());
	return totalcount;
}
int InstanceCategorizer::GetCount(const std::string& className)
{
	return int(_map[className].size());
}

std::vector<int> InstanceCategorizer::GetEntropyVector() const
{
	std::vector<int> v;
	for (auto& it : _map)
	{
		if (it.second.size() > 0)
			v.emplace_back(int(it.second.size()));
	}
	return std::move(v);
}

std::vector<Instance*> InstanceCategorizer::GetInstances() const
{
	std::vector<Instance*> instances;
	for (auto inst : _map)
	{
		for (auto it : inst.second)
			instances.push_back(it);
	}
	return std::move(instances);
}

std::vector<Instance*> InstanceCategorizer::GetInstances(
	const std::string& classname)
{
	return _map[classname];
}

std::vector<std::string> InstanceCategorizer::GetClasses() const
{
	std::vector<std::string> classes;
	for (auto inst : _map)
		classes.push_back(inst.first);
	return std::move(classes);
}
