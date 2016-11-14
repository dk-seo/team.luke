#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Instance;

struct ClassCounter
{
	void Inc(Instance* instance, size_t att);

	int Get(const std::string& classname);

	std::vector<int> GetVector() const;

	std::vector<Instance*> GetInstances() const;

	std::vector<std::string> GetClasses() const;

private:
	std::unordered_map<std::string, std::vector<Instance*>> map;
};