#include "RandomSampler.h"
#include "../Dataframe/Dataframe.h"
#include <random>

void RandomSampler::Sample(Dataframe& dataframe, int sampleSize)
{
	_samples.clear();

  //if(dataframe.GetInstanceCount() == 1)

	// a checker that marks instance that has already been marked
	std::vector<bool> sampledCheck;
	sampledCheck.resize(dataframe.GetInstanceCount(), false);

	std::random_device rd;
	std::default_random_engine randomGenerator(rd());
	std::uniform_int_distribution<int> distribution(
		0, int(dataframe.GetInstanceCount()) - 1);

	// until designated number of instances are sampled
	int sampledSize = 0;
	while (sampledSize < sampleSize)
	{
		// roll dice
		int roll = distribution(randomGenerator);
		if (sampledCheck[roll])
			continue;

		// save to sampledInstances
		sampledCheck[roll] = true;
		_samples.emplace_back(&dataframe.GetInstance(roll));
		++sampledSize;
	}
}
