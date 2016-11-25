#pragma once
#include "ML\Dataframe\Dataframe.h"
#include "ML\Regression\LinearRegression.h"

class LinearRegressionTest
{
public:

    LinearRegressionTest(Dataframe *_dataframe);
    ~LinearRegressionTest(void);

    void Answer(std::vector<Equation>& bestfits, std::vector<double>& errors, size_t& max, size_t& min);

private:
    Dataframe *m_Dataframe;

};