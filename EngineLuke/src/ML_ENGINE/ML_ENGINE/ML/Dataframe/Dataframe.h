#pragma once
#include <string>
#include <vector>
#include <cstdlib>
#include <unordered_map>

namespace AttributeType
{
	enum Enum
	{
		Nominal,
		Numeric
	};
}

// Attribute.
// In order to make room for reading in many other forms than string.
class Attribute
{
public:
	Attribute(const std::string& content);

	bool IsMissing() const { return _content == "?"; }

	double AsDouble() const { return atof(_content.c_str()); }
	const std::string& AsString() const { return _content; }

	void Set(const std::string& value) { _content = value; }
private:
	std::string _content;
};

// Instance
class Instance
{
public:
	int GetAttributeCount() const;
	Attribute& GetAttribute(size_t idx);
	const Attribute& GetAttribute(size_t idx) const;
	void AddAttribute(const std::string& attribute);

private:
	std::vector<Attribute> _attributes;
};

class Dataframe
{
public:
	Dataframe() {}
	~Dataframe();

	Dataframe(Dataframe&& rhs);

	Dataframe& operator=(Dataframe&& rhs);

	Dataframe Clone();

	// build dataframe from csv file
	// if fails, call GetErrorMessage to check the error
	bool BuildFromCsv(
		const std::string& filename,
		bool hasHeader,
		const std::vector<size_t>& selectedFeatures = {});

	// Add an attribute with name
	void Dataframe::AddAttribute(
		const std::string& attributeName, 
		AttributeType::Enum type);

	// get name of ith attribute
	const std::string& GetAttributeName(size_t i) const;

	// get type of ith attribute
	AttributeType::Enum GetAttributeType(size_t i) const;

	void SetAttributeType(size_t i, AttributeType::Enum type) {
		_attributeTypes[i] = type; }

	// get number of attributes
	size_t GetAttributeCount() const;

	// Get all the instances
	const std::vector<Instance*>& GetInstances() const { return _instances; }

	// Get attribute of a specific instance
	const Attribute& GetInstanceAttribute(
		const Instance* instance, const std::string& attName);

	// get instance count
	size_t GetInstanceCount() const;

	// get instance of an index
	Instance& GetInstance(size_t idx);
	const Instance& GetInstance(size_t idx) const;

	// get error message
	std::string GetErrorMessage() const;

	// Merge two data frames (rhs get invalidated after this operation)
	bool Merge(Dataframe& rhs);

	// exports in csv format to o
	void ToCsv(std::ostream& o);


private:
	Instance* CreateInstance();

private:
	typedef std::unordered_map<std::string, unsigned> AttributeMap;

private:
	std::vector<std::string> _attributeNames;
	std::vector<AttributeType::Enum> _attributeTypes;
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
