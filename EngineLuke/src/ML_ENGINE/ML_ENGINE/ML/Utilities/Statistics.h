#pragma once
#include <vector>
class Instance;

double CalculateMean(
	const std::vector<Instance*>& instances, size_t attributeIdx);

double CalculateMedian(
	std::vector<Instance*> instances, size_t attributeIdx);

std::vector<double> CalculateMode(
	const std::vector<Instance*>& instances, size_t attributeIdx);

double FindMax(
	const std::vector<Instance*>& instances, size_t attributeIdx);

double FindMin(
	const std::vector<Instance*>& instances, size_t attributeIdx);