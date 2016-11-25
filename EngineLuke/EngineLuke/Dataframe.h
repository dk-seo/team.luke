#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>

// Attribute.
// In order to make room for reading in many other forms than string.
class Attribute
{
public:
	Attribute(const std::string& content);

	bool IsMissing() const { return _content == "?"; }

	double AsDouble() const { return atof(_content.c_str()); }
	const std::string& AsString() const { return _content; }
private:
	std::string _content;
};

// Instance
class Instance
{
public:
	int GetAttributeCount() const;
	const Attribute& GetAttribute(size_t idx) const;

private:
	std::vector<Attribute> _attributes;

private:
	friend class InstanceBuilder;
	
	void AddAttribute(const std::string& attribute);
};

// This class Helps build instance in private way
// so that once built the instance
// cannot be modified any more (as its public interface
// doesn't allow it)
class InstanceBuilder
{
public:
	InstanceBuilder(const InstanceBuilder& rhs);

	// add attribute for instance. It checks attribute limit.
	bool AddAttribute(const std::string& attribute);

private:
	Instance& _instance;
	int _attributeCount;

private:
	friend class Dataframe;
	InstanceBuilder(Instance& instance, int attributeCount);
};	

class Dataframe
{
public:
	~Dataframe();

	// build dataframe from csv file
	// if fails, call GetErrorMessage to check the error
	bool BuildFromCsv(const std::string& filename, bool hasHeader);

	// get attribute name of index i
	const std::string& GetAttributeName(size_t i);

	// get number of attributes
	size_t GetAttributeCount() const;

	// Get all the instances
	const std::vector<Instance*>& GetInstances() const { return _instances; }

	// Get attribute of a specific instance
	const Attribute& GetInstanceAttribute(const Instance* instance, const std::string& attName);

	// get instance count
	size_t GetInstanceCount() const;

  // get attribute index
  int GetAttributeIndex(std::string & name);

	// get instance of an index
	const Instance& GetInstance(size_t idx);

	// get error message
	std::string GetErrorMessage() const;

private:
	// Create Instance and returns instance builder.
	// InstanceBuilder was introduced at first to open this api to public,
	// but decided to hide it after implementing BuildFromCsv.
	// I am leaving this as it is in case I have to open this api to public at
	// later expansion.
	InstanceBuilder CreateInstance();

	// Add an attribute with name
	void AddAttribute(const std::string& attributeName);

private:
	std::vector<std::string> _attributes;
	typedef std::unordered_map<std::string, unsigned> AttributeMap;
	AttributeMap _attributeMap;
	std::vector<Instance*> _instances;
	std::string _errormsg;
};

template<typename Func>
static std::vector<Instance*> Filter(const std::vector<Instance*>& original, Func& filter)
{
	std::vector<Instance*> filtered;
	for (const auto& i : original)
	{
		if (filter(i))
			filtered.emplace_back(i);
	}
	return std::move(filtered);
}
