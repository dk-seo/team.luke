#include "sampling.h"

namespace CS399Sampling
{

	Sampling::Sampling(void)
	{
	}

	Sampling::~Sampling(void)
	{
	}

	std::vector<int> Sampling::GetSimpleRandomNums(int population, int sample)
	{
		std::vector<int> result;
		int index = 0;
		bool again = false;
		srand(unsigned(time(NULL)));
		while (index < sample)
		{
			// Get random number
			int num = rand() & population;

			// First random number
			if (result.size() == 0)
			{
				result.push_back(num); 
				++index; 
				continue;
			}

			// Check if the random number is already in the array
			unsigned i = 0;
			for (i = 0; i < result.size(); ++i)
			{
				if (result[i] == num)
				{
					again = true;
					break;
				}
			}
			
			// Insert into the result array
			if (!again)
			{
				result.push_back(num);
				++index;
			}
			else
				again = false;
		}

		return result;
	}


	int* Sampling::GetReserviorNums(int streamMaxindex, int n, int k)
	{
		// Initial stream array with index
		int* stream = new int[streamMaxindex];
		for (int i = 0; i < streamMaxindex; ++i)
			stream[i] = i;

		int index;
		int* reservior = new int[k];
		
		// Create an reservior array and copy first k items of stream array
		for (index = 0; index < k; ++index)
			reservior[index] = stream[index];

		srand(unsigned(time(NULL)));

		for (; index < n; ++index)
		{
			// Get random number
			int j = rand() % (index + 1);

			// Replace reservior with stream
			if (j < k)
				reservior[j] = stream[index];
		}

		return reservior;
	}

	double Sampling::GetMean(const double* list, int size)
	{
		double sum = 0;

		// Sum of data
		for (int i = 0; i < size; ++i)
			sum += list[i];
		
		// Result
		return sum / size;
	}

	double Sampling::GetMedian(const double* list, int size)
	{
		// Sort array!!
		double* sortList = new double[size];

		for (int i = 0; i < size; ++i)
			sortList[i] = list[i];
		
		for (int i = size - 1; i > 0; --i)
		{
			for (int j = 0; j < i; ++j) 
			{
				if (sortList[j] > sortList[j + 1]) 
				{
					double temp = sortList[j];
					sortList[j] = sortList[j + 1];
					sortList[j + 1] = temp;
				}
			}
		}

		// Middle or average of middle values in the sorted array.
		double median = 0.0;
		if ((size % 2) == 0) 
			median = (sortList[size / 2] + sortList[(size / 2) - 1]) / 2.0;
		else
			median = sortList[size / 2];
		
		delete[] sortList;
		return median;
	}

	double Sampling::GetMode(const double* list, int size)
	{
		int* repetition = new int[size];
		for (int i = 0; i < size; ++i) 
		{
			repetition[i] = 0;
			int j = 0;
			while ((j < i) && (list[i] != list[j])) 
			{
				if (list[i] != list[j]) 
					++j;
			}
			++(repetition[j]);
		}

		// Find max appeared data
		int max = 0;
		for (int i = 1; i < size; ++i) 
		{
			if (repetition[i] > repetition[max])
				max = i;
		}

		delete[] repetition;
		return list[max];
	}
}
