#pragma once
#include"Vector.h"

class Instance;

struct Equation
{
	float a; // Slope of Line
	float b; // Y intercept

	Equation();
	
	Equation(float _a, float _b);
	
	Equation(Vector2 _point1, Vector2 _point2);
	
	Equation(Instance& _attr, Instance& _class);

	float Distance(const Vector2& _point);
};