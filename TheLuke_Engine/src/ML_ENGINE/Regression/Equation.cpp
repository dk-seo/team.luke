#include "Equation.h"
#include "../Dataframe/Dataframe.h"



Equation::Equation()
{
}

Equation::Equation(float _a, float _b)
	:a(_a)
	,b(_b)
{
}

Equation::Equation(Vector2 _point1, Vector2 _point2)
{
	a = (_point2.y - _point1.y) / (_point2.x - _point1.x);
	b = _point1.x * a + _point1.y;
}

Equation::Equation(Instance & _attr, Instance & _class)
{
	int size = _attr.GetAttributeCount();

	float sum_x  = 0;
	float sum_y  = 0;
	float sum_xx = 0;
	float sum_yy = 0;
	float sum_xy = 0;

	for (int i = 0; i < size; ++i)
	{
		int x = _attr.GetAttribute(i).AsDouble();
		int y = _class.GetAttribute(i).AsDouble();

		sum_x += x;
		sum_y += y;

		sum_xx += (x * x);
		sum_yy += (y * y);
		sum_xy += (x * y);
	}

	a = (size*sum_xy - sum_x*sum_y) / (size*sum_xx - sum_x*sum_x);
	b = (sum_xx*sum_y - sum_x*sum_xy) / (size*sum_xx - sum_x*sum_x); 
}

float Equation::Distance(const Vector2 & _point)
{
	return std::abs(a*_point.x + _point.y + b) / std::sqrt(a*a + 1);
}
