#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class MultiIntegralDiscretizer;

class Instance;

class InstanceCategorizer
{
public:
	InstanceCategorizer(
		size_t idx, MultiIntegralDiscretizer* discretizer = nullptr);
	InstanceCategorizer(InstanceCategorizer&& rhs);
	~InstanceCategorizer();
	InstanceCategorizer& operator=(InstanceCategorizer&& rhs);

	void Add(Instance* instance);

	int GetCount(const std::string& classname);
	int GetCount();

	std::vector<int> GetEntropyVector() const;

	std::vector<Instance*> GetInstances() const;

	std::vector<Instance*> GetInstances(const std::string& classname);

	std::vector<std::string> GetClasses() const;

	MultiIntegralDiscretizer* GetDiscretizer() { return _discretizer.get(); }

	MultiIntegralDiscretizer* ReleaseDiscretizer() { return _discretizer.release(); }

	size_t GetClassCount() const { return _map.size(); }

private:
	size_t _attributeIdx;
	std::unordered_map<std::string, std::vector<Instance*>> _map;
	std::unique_ptr<MultiIntegralDiscretizer> _discretizer;
};