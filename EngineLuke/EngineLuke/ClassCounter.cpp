#include "ClassCounter.h"
#include "Dataframe.h"

std::vector<std::string> ClassCounter::GetClasses() const
{
	std::vector<std::string> classes;
	for (auto inst : map)
		classes.push_back(inst.first);
	return std::move(classes);
}

void ClassCounter::Inc(Instance* instance, size_t att)
{
	std::string className = instance->GetAttribute(att).AsString();
	map[className].emplace_back(instance);
}

int ClassCounter::Get(const std::string& className)
{
	return int(map[className].size());
}

std::vector<int> ClassCounter::GetVector() const
{
	std::vector<int> v;
	for (auto& it : map)
	{
		if (it.second.size() > 0)
			v.emplace_back(int(it.second.size()));
	}
	return std::move(v);
}

std::vector<Instance*> ClassCounter::GetInstances() const
{
	std::vector<Instance*> instances;
	for (auto inst : map)
	{
		for (auto it : inst.second)
			instances.push_back(it);
	}
	return std::move(instances);
}