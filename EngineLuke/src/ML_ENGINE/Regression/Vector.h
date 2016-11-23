#pragma once

struct Vector2
{
	Vector2() {};
	Vector2(float x, float y);

	/////////////////////////////////////
	float x, y;
	float arr[2];

	/////////////////////////////////////
	//Operator

	float& operator[](unsigned index);
	
	float operator[](unsigned index) const;

	Vector2 operator+(const Vector2& rhs) const;
	
	Vector2 operator-(const Vector2& rhs) const;
	
	Vector2 operator*(const float rhs) const;
	
	Vector2 operator/(const float rhs) const;

	void operator+=(const Vector2& rhs);
	
	void operator-=(const Vector2& rhs);
	
	void operator*=(const float rhs);
	
	void operator/=(const float rhs);

	bool operator==(const Vector2& rhs) const;
	
	bool operator!=(const Vector2& rhs) const;

	float Dot(const Vector2& rhs) const;
	
	float Length(void) const;
};