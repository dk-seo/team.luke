#include "LinearRegression.h"
#include "../Dataframe/Dataframe.h"

Regression::Regression(Dataframe & dataframe)
	:m_dataframe(dataframe)
{
}

Regression::~Regression(void)
{
}

Equation Regression::GetBestFitLine(Instance & _attr, Instance & _class)
{
	return Equation(_attr, _class);
}

double Regression::SumError(Equation& line, Instance& _attr, Instance& _class)
{
	int size = _attr.GetAttributeCount();
	
	double sum = 0;
	
	for (int i = 0; i < size; ++i)
	{
		sum += line.Distance(Vector2(_attr.GetAttribute(i).AsDouble(), _attr.GetAttribute(i).AsDouble()));
	}
	
	return sum;
}
