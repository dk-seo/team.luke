#include "LinearRegressionTest.h"
#include "ML\Regression\Equation.h"

#include <vector>

LinearRegressionTest::LinearRegressionTest(Dataframe *_dataframe)
{
    m_Dataframe = _dataframe;
}

LinearRegressionTest::~LinearRegressionTest(void)
{
}

void LinearRegressionTest::Answer(std::vector<Equation>& bestfits
                                , std::vector<double>& errors
                                , size_t& _max
                                , size_t& _min)
{
    Regression *regression = new Regression();

    double max = 0, min = 0;
    size_t i_max = -1, i_min = -1;

    size_t size = m_Dataframe->GetAttributeCount();

    for (int i = 0; i < size - 1; ++i)
    {
        Equation bestfit = regression->GetBestFitLine(m_Dataframe->GetInstance(i)
                                                     ,m_Dataframe->GetInstance(size));
        bestfits.push_back(bestfit);
        double error = regression->SumError(bestfit
                                           ,m_Dataframe->GetInstance(i)
                                           ,m_Dataframe->GetInstance(size));
        errors.push_back(error);
        
        if (i == 0)
        {
            max = min = error;
            i_max = i_min = i;
        }
        else
        {
            if (max < error)
            {
                max = error;
                i_max = i;
            }
            if (min > error)
            {
                min = error;
                i_min = i;
            }
        }
    }

    _max = i_max;
    _min = i_min;
    delete regression;
}

