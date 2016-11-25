#include "Dataframe.h"
#include "csvparser.h"
#include <cassert>
/////////////////////////////////////////////////////////////// 
// attribute

Attribute::Attribute(const std::string& content)
	: _content(content)
{
}

/////////////////////////////////////////////////////////////// 
// Instance

int Instance::GetAttributeCount() const
{
	return int(_attributes.size());
}

const Attribute& Instance::GetAttribute(size_t idx) const
{
	return _attributes[idx];
}

void Instance::AddAttribute(const std::string& attribute)
{
	_attributes.push_back(Attribute(attribute));
}

InstanceBuilder::InstanceBuilder(Instance& instance, int attributeCount)
	:_instance(instance), _attributeCount(attributeCount)
{
}


/////////////////////////////////////////////////////////////// 
// Instance Builder
InstanceBuilder::InstanceBuilder(const InstanceBuilder& rhs)
	: _instance(rhs._instance), _attributeCount(rhs._attributeCount)
{
}

bool InstanceBuilder::AddAttribute(const std::string& attribute)
{
	if (_instance.GetAttributeCount() >= _attributeCount)
		return false;

	_instance.AddAttribute(attribute);
	return true;
}


/////////////////////////////////////////////////////////////// 
// Dataframe

Dataframe::~Dataframe()
{
	for (auto instance : _instances)
		delete instance;
}
size_t Dataframe::GetAttributeCount() const
{
	return _attributes.size();
}


bool Dataframe::BuildFromCsv(const std::string& filename, bool hasHeader)
{
	// I am using open csv parser.
	CsvParser *csvParser = CsvParser_new(filename.c_str(), ",", int(hasHeader));

	CsvRow* header = CsvParser_getHeader(csvParser);
	if (!header)
	{
		_errormsg = CsvParser_getErrorMessage(csvParser);
		return false;
	}

	// build attribute types
	int attributeCount = CsvParser_getNumFields(header);
	const char **parsedHeader = CsvParser_getFields(header);
	for (int i = 0; i < attributeCount; ++i)
	{
		AddAttribute(parsedHeader[i]);
	}

	// build instances
	while (CsvRow* row = CsvParser_getRow(csvParser))
	{
		const char **parsedRow = CsvParser_getFields(row);
		InstanceBuilder builder = CreateInstance();
		for (int i = 0; i < attributeCount; ++i)
		{
			if (!builder.AddAttribute(parsedRow[i]))
			{
				_errormsg = "No!! Number of attribute exceeded!";
				return false;
			}
		}
	}

	_errormsg = "";

	CsvParser_destroy(csvParser);
	return true;
}

const std::string& Dataframe::GetAttributeName(size_t i)
{
	assert(i < GetAttributeCount());
	return _attributes[i];
}

InstanceBuilder Dataframe::CreateInstance()
{
	_instances.emplace_back(new Instance);
	return InstanceBuilder(*_instances.back(), int(_attributes.size()));
}

void Dataframe::AddAttribute(const std::string& attributeName)
{
	_attributes.emplace_back(attributeName);
	_attributeMap.emplace(attributeName, (unsigned)_attributes.size() - 1);
}

const Attribute& Dataframe::GetInstanceAttribute(const Instance* instance, const std::string& attName)
{
	return instance->GetAttribute(int(_attributeMap[attName]));
}

size_t Dataframe::GetInstanceCount() const
{
	return int(_instances.size());
}

int Dataframe::GetAttributeIndex(std::string & name)
{
  return static_cast<int>(_attributeMap[name]);
}

const Instance& Dataframe::GetInstance(size_t idx)
{
	return *_instances[idx];
}

std::string Dataframe::GetErrorMessage() const
{
	return _errormsg;
}
