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
	void AddAttribute(const std::string& attribute);

private:
	std::vector<Attribute> _attributes;
};

class Dataframe
{
public:
	~Dataframe();

	// build dataframe from csv file
	// if fails, call GetErrorMessage to check the error
	bool BuildFromCsv(const std::string& filename, bool hasHeader);

	// Add an attribute with name
	void AddAttribute(const std::string& attributeName);

	// get name of ith column attribute
	const std::string& GetAttributeName(size_t i);

	// get number of attributes
	size_t GetAttributeCount() const;

	// Get all the instances
	const std::vector<Instance*>& GetInstances() const { return _instances; }

	// Get attribute of a specific instance
	const Attribute& GetInstanceAttribute(const Instance* instance, const std::string& attName);

	// get instance count
	size_t GetInstanceCount() const;

	// get instance of an index
	const Instance& GetInstance(size_t idx);

	// get error message
	std::string GetErrorMessage() const;

	// Merge two data frames (rhs get invalidated after this operation)
	bool Merge(Dataframe& rhs);

	void ToCsv(std::ostream& o);

private:
	Instance* CreateInstance();

private:
	typedef std::unordered_map<std::string, unsigned> AttributeMap;

private:
	std::vector<std::string> _attributeNames;
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
