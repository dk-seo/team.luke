#pragma once

struct Vector2
{
	Vector2() {};
    Vector2(double _x, double _y);

	/////////////////////////////////////
	double x, y;
	double arr[2];

	/////////////////////////////////////
	//Operator

	double& operator[](unsigned index);
	
    double operator[](unsigned index) const;

	Vector2 operator+(const Vector2& rhs) const;
	
	Vector2 operator-(const Vector2& rhs) const;
	
	Vector2 operator*(const float rhs) const;
	
	Vector2 operator/(const float rhs) const;

	void operator+=(const Vector2& rhs);
	
	void operator-=(const Vector2& rhs);
	
	void operator*=(const double rhs);
	
	void operator/=(const double rhs);

	bool operator==(const Vector2& rhs) const;
	
	bool operator!=(const Vector2& rhs) const;

    double Dot(const Vector2& rhs) const;
	
    double Length(void) const;
};