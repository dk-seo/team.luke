#include "ClassCounter.h"
#include "Dataframe.h"

ClassCounter::ClassCounter(size_t attIdx)
	:_attributeIdx(attIdx)
{
}

void ClassCounter::Inc(Instance* instance)
{
	std::string className = instance->GetAttribute(_attributeIdx).AsString();
	_map[className].emplace_back(instance);
}

int ClassCounter::Get(const std::string& className)
{
	return int(_map[className].size());
}

std::vector<int> ClassCounter::GetEntropyVector() const
{
	std::vector<int> v;
	for (auto& it : _map)
	{
		if (it.second.size() > 0)
			v.emplace_back(int(it.second.size()));
	}
	return std::move(v);
}

std::vector<Instance*> ClassCounter::GetInstances() const
{
	std::vector<Instance*> instances;
	for (auto inst : _map)
	{
		for (auto it : inst.second)
			instances.push_back(it);
	}
	return std::move(instances);
}

std::vector<Instance*> ClassCounter::GetInstances(
	const std::string& classname)
{
	return _map[classname];
}

std::vector<std::string> ClassCounter::GetClasses() const
{
	std::vector<std::string> classes;
	for (auto inst : _map)
		classes.push_back(inst.first);
	return std::move(classes);
}
