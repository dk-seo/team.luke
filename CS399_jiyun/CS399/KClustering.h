#pragma once

#include "def.h"

class KClustering
{
public:

	KClustering(void);

	~KClustering(void);

	std::vector<XYcoord> DoClustering(DATASET* dataset, int k);

private:
    //////////////////////////////////////////////////////////////////////////
    // Private Method

    bool Clustring(DATASET* dataset, int size);

    void Coloring(XYcoord coordinate, int index);

    bool check(void);

	void sortlist(void);

    // For random centroid at the first time 
    void RandomIndex(DATASET* dataset, int pop, int k);
    
    std::vector<int> GetSimpleRandomNums(int population, int sample);

    // Clear functions
    void ClearColoring(int n);
    
    void ClearList(int k);

    //////////////////////////////////////////////////////////////////////////
    // Private variables
    std::vector<int>     m_coloring;      // for data coloring
    std::vector<XYcoord> m_centroid;      // centorid list
   
    // For avg calculation
    std::vector<XYcoord> m_sumcontainer;  // this is actually coordinate
    std::vector<int>     m_totalnumList;  // this is number of data
};