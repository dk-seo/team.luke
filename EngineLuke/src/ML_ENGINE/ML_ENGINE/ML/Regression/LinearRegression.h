#pragma once
#include <vector>
#include <string>

#include "Equation.h"

class Instance;
class Dataframe;

class Regression
{
public:

	Regression(Dataframe& dataframe);

	~Regression(void);

	Equation GetBestFitLine(Instance& _attr, Instance& _class);

	double SumError(Equation& line, Instance& _attr, Instance& _class);

private:
	Dataframe& m_dataframe;

};