#pragma once
#include <vector>
#include <string>
#include <ostream>

double Gain(
	std::ostream* _o,
	const std::string& name,
	const std::vector<int>& vector,
	const std::vector<std::string>& childrenNames,
	const std::vector<std::vector<int>>& children,
	bool printMetadata,
	std::string tabs);