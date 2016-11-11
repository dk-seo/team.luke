#pragma once

#include "def.h"

namespace CS399Sampling
{
	class Sampling
	{
	public:
		Sampling(void);
		~Sampling(void);

		// Getter functions
		
		std::vector<int> GetSimpleRandomNums(int population, int sample);
		int* GetReserviorNums(int streamMaxindex, int n, int k);
		double GetMean(const double* list, int size);
		double GetMedian(const double* list, int size);
		double GetMode(const double* list, int size);

	};
}
