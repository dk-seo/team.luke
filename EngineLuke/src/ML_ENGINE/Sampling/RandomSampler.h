#pragma once
#include <vector>

class Dataframe;
class Instance;

class RandomSampler
{
public:
	void Sample(Dataframe& dataframe, int sampleSize);

	const std::vector<const Instance*>& GetSamples() { return _samples; }

private:
	std::vector<const Instance*> _samples;
};
