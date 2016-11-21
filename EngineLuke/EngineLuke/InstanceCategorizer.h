#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Instance;

struct InstanceCategorizer
{
	InstanceCategorizer(size_t idx);

	void Inc(Instance* instance);

	int Get(const std::string& classname);

	std::vector<int> GetEntropyVector() const;

	std::vector<Instance*> GetInstances() const;
	
	std::vector<Instance*> GetInstances(const std::string& classname);

	std::vector<std::string> GetClasses() const;

	size_t GetClassCount() const { return _map.size(); }

private:
	size_t _attributeIdx;
	std::unordered_map<std::string, std::vector<Instance*>> _map;
};