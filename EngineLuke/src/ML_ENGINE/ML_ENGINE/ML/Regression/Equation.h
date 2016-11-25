#pragma once
#include"Vector.h"

class Instance;

struct Equation
{
	double a; // Slope of Line
	double b; // Y intercept

	Equation();
	
	Equation(double _a, double _b);
	
	Equation(Vector2 _point1, Vector2 _point2);
	
	Equation(Instance& _attr, Instance& _class);

	double Distance(const Vector2& _point);
};