#include "Dataframe.h"
#include "../../FileIO/csvparser.h"
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

Attribute& Instance::GetAttribute(size_t idx)
{
	return _attributes[idx];
}
const Attribute& Instance::GetAttribute(size_t idx) const
{
	return _attributes[idx];
}

void Instance::AddAttribute(const std::string& attribute)
{
	_attributes.push_back(Attribute(attribute));
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
	return _attributeNames.size();
}

Dataframe::Dataframe(Dataframe&& rhs)
{
	_attributeMap = std::move(rhs._attributeMap);
	_attributeNames = std::move(rhs._attributeNames);
	_attributeTypes = std::move(rhs._attributeTypes);
	_instances = std::move(rhs._instances); rhs._instances.clear();
}

Dataframe& Dataframe::operator=(Dataframe&& rhs)
{
	_attributeMap = std::move(rhs._attributeMap);
	_attributeNames = std::move(rhs._attributeNames);
	_attributeTypes = std::move(rhs._attributeTypes);
	_instances = std::move(rhs._instances); rhs._instances.clear();
	return *this;
}

Dataframe Dataframe::Clone()
{
	Dataframe clone;
	clone._attributeNames = _attributeNames;
	clone._attributeMap = _attributeMap;
	clone._attributeTypes = _attributeTypes;
	clone._instances.reserve(_instances.size());
	for (auto instance : _instances)
	{
		Instance* inst = clone.CreateInstance();
		*inst = *instance;
	}
	return std::move(clone);
}

bool Dataframe::BuildFromCsv(
	const std::string& filename,
	bool hasHeader,
	const std::vector<size_t>& selectedFeatures)
{
	// I am using open-source csv parser.
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
	if (selectedFeatures.empty())
	{
		for (int i = 0; i < attributeCount; ++i)
			AddAttribute(parsedHeader[i], AttributeType::Nominal); // nominal by default
	}
	else
	{
		auto& selectedIt = selectedFeatures.begin();
		for (int i = 0; i < attributeCount; ++i)
		{
			if (selectedIt != selectedFeatures.end() && *selectedIt == i)
			{
				AddAttribute(parsedHeader[i], AttributeType::Nominal); // nominal by default
				++selectedIt;
			}
		}
	}
	// build instances
	while (CsvRow* row = CsvParser_getRow(csvParser))
	{
		const char **parsedRow = CsvParser_getFields(row);
		Instance* instance = CreateInstance();
		if (selectedFeatures.empty())
		{
			for (int i = 0; i < attributeCount; ++i)
				instance->AddAttribute(parsedRow[i]);
		}
		else
		{
			auto selectedIt = selectedFeatures.begin();
			for (int i = 0; i < attributeCount; ++i)
			{
				if (selectedIt != selectedFeatures.end() && *selectedIt == i)
				{
					instance->AddAttribute(parsedRow[i]);
					++selectedIt;
				}
			}
		}
		
	}

	_errormsg = "";

	CsvParser_destroy(csvParser);
	return true;
}

const std::string& Dataframe::GetAttributeName(size_t i) const
{
	assert(i < GetAttributeCount());
	return _attributeNames[i];
}

AttributeType::Enum Dataframe::GetAttributeType(size_t i) const
{
	assert(i < GetAttributeCount());
	return _attributeTypes[i];
}

Instance* Dataframe::CreateInstance()
{
	_instances.emplace_back(new Instance);
	return _instances.back();
}

void Dataframe::AddAttribute(
	const std::string& attributeName, AttributeType::Enum type)
{
	_attributeNames.emplace_back(attributeName);
	_attributeTypes.emplace_back(type);
	_attributeMap.emplace(attributeName, (unsigned)_attributeNames.size() - 1);
}

const Attribute& Dataframe::GetInstanceAttribute(
	const Instance* instance, const std::string& attName)
{
	return instance->GetAttribute(int(_attributeMap[attName]));
}

size_t Dataframe::GetInstanceCount() const
{
	return int(_instances.size());
}

Instance& Dataframe::GetInstance(size_t idx)
{
	return *_instances[idx];
}
const Instance& Dataframe::GetInstance(size_t idx) const
{
	return *_instances[idx];
}
std::string Dataframe::GetErrorMessage() const
{
	return _errormsg;
}

bool Dataframe::Merge(Dataframe& rhs)
{
	if (_attributeNames != rhs._attributeNames)
		return false;

	_instances.insert(
		_instances.end(),
		std::make_move_iterator(rhs._instances.begin()),
		std::make_move_iterator(rhs._instances.end()));
	rhs._instances.clear();
	return true;

}

void Dataframe::ToCsv(std::ostream& o)
{
	for (size_t i = 0; i < _attributeNames.size(); ++i)
	{
		if (i > 0)
			o << ",";
		o << _attributeNames[i];
	}
	o << std::endl;

	for (auto instance : _instances)
	{
		for (size_t i = 0; i < _attributeNames.size(); ++i)
		{
			if (i > 0)
				o << ",";
			o << instance->GetAttribute(i).AsString();
		}
		o << std::endl;
	}
}