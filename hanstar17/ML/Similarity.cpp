#include "Similarity.h"
#include <cmath>

static double sq(const double& a)
{
	return a*a;
}

static double dot(const std::vector<double>& a, const std::vector<double>& b)
{
	double result = 0.0;
	for (size_t i = 0; i < a.size(); ++i)
		result += sq(a[i] - b[i]);
	return result;
}

double EuclideanDistanceSimilarity(
	const std::vector<double>& a, const std::vector<double>& b)
{
	return 1.0 / (1.0+sqrt(dot(a, b)));
}