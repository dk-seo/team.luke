#pragma once

#include <vector>
#include <string>

#include <time.h>
#include <algorithm>  

#include <fstream>
#include <stdlib.h>
#include <iostream>
#include "csvparser.h"

#define DATASET std::vector<std::vector<std::string>>

#define MikeDataSetSize 316
#define ChicagoDataSetSize 32063

typedef std::vector<std::string>      BData_Name;
typedef std::vector<int>              BData_Salary;

////////////////////////////////////////////////////////////////
//OneRule
struct Result
{
    std::string result;
    int appearence = 1;
};

struct Rule
{
    std::string attr_data;
    std::vector<Result> fin;
    int maxindex = 0;
};
struct RuleClass
{
    std::string attr;
    std::vector<Rule> rules;
    int totalerror = 0;
};



////////////////////////////////////////////////////////////////
struct XYcoord
{
	double X; //height
    double Y; //weight

    XYcoord() { X = 0; Y = 0; }
	XYcoord(double x, double y) : X(x), Y(y) {}

    XYcoord& operator+=(const XYcoord& rhs) 
    { 
        X += rhs.X;
        Y += rhs.Y;
        return *(XYcoord*)(this);
    }

    XYcoord& operator/(const double& rhs)
    {
        X /= rhs;
        Y /= rhs;
        return *(XYcoord*)(this);
    }
	bool operator==(const XYcoord& rhs)
	{
		if (X == rhs.X && Y == rhs.Y)
			return true;
		else
			return false;
	}
};

