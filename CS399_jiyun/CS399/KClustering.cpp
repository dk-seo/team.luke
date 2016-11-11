#include "KClustering.h"

bool compare_coord(XYcoord a, XYcoord b)
{
	if (int(a.X * 100) == int(b.X * 100))
		return (a.Y < b.Y);
	else
		return (a.X < b.X);
}

KClustering::KClustering(void)
{
    m_coloring.clear();
    m_centroid.clear();
}
KClustering::~KClustering(void)
{
    m_coloring.clear();
    m_centroid.clear();
    m_sumcontainer.clear();
    m_totalnumList.clear();
}

std::vector<XYcoord> KClustering::DoClustering(DATASET* dataset, int k)
{
    int n = int(dataset[0][0].size() - 1);
    
    // Clear the lists
    ClearList(k);
    ClearColoring(n);

    // Get a random centroid
    RandomIndex(dataset, n, k);

    // while ending condition
    bool finished = false;

    while (!finished)
    {
        finished = Clustring(dataset, n);  // Do clustring
        ClearList(k);                      // Clear the list for next step
    }

    // Resutn result centroids 
    return m_centroid;
}

void KClustering::RandomIndex(DATASET* dataset, int pop, int k)
{
    m_centroid.resize(k);

    //Get random indexes from dataset
    std::vector<int> indexlist = GetSimpleRandomNums(pop, k);

    //Get XY values from the dataset
    for (int i = 0; i < k; ++i)
    {
        std::string x = (*dataset)[1][indexlist[i]];
        std::string y = (*dataset)[2][indexlist[i]];

        //Insert
        m_centroid[i] = XYcoord(atof(x.c_str()), atof(y.c_str()));
    }
}

std::vector<int> KClustering::GetSimpleRandomNums(int population, int sample)
{
    std::vector<int> result;
    int index = 0;
    bool again = false;
    srand(unsigned(time(NULL)));
    while (index < sample)
    {
        // Get random number
        int num = rand() & population;

        // First random number
        if (result.size() == 0)
        {
            result.push_back(num);
            ++index;
            continue;
        }

        // Check if the random number is already in the array
        unsigned i = 0;
        for (i = 0; i < result.size(); ++i)
        {
            if (result[i] == num)
            {
                again = true;
                break;
            }
        }

		if (num == 0)
			again = true;

        // Insert into the result array
        if (!again)
        {
            result.push_back(num);
            ++index;
        }
        else
            again = false;
    }

    return result;
}

void KClustering::ClearColoring(int n)
{
    m_coloring.clear();       // clear the coloring list

    m_coloring.resize(n + 1); // resize the lit 

    m_coloring[0] = -1;       // Insert any value for head
}

bool KClustering::Clustring(DATASET* dataset, int size)
{
    // Coloring all data from the dataset
    for (int i = 1; i <= size; ++i)
    {
        // Get coordinate 
        std::string x = (*dataset)[1][i];
        std::string y = (*dataset)[2][i];
        XYcoord coord = XYcoord(atof(x.c_str()), atof(y.c_str()));

        // Do coloring
        Coloring(coord, i);
    }

    //Calculate new centroid
    int length = int(m_sumcontainer.size());
    for (int i = 0; i < length; ++i)
    {
        // Get a avg all points that has same color, and get new centroid
		if(m_totalnumList[i] !=0)
			m_sumcontainer[i] / float(m_totalnumList[i]);
    }

    // Sort list for easy compare 
	sortlist();

    // Check new centroids are same with previous centroid
    // This will be ending condition, if return true
    return check();
}

void KClustering::Coloring(XYcoord coordinate, int index)
{
    int size = int(m_centroid.size());
    double dis = 10000000;
    int coloringindex = 0;

    // Compare all points with centroids
	for (int i = 0; i < size; ++i)
	{
        // If point is same with centroid
		if (m_centroid[i] == coordinate)
		{
			coloringindex = i;
			break;
		}
        // Otherwise
		else
		{
            // Temp distance 
			double temp = std::sqrt((m_centroid[i].X - coordinate.X)*(m_centroid[i].X - coordinate.X) + (m_centroid[i].Y - coordinate.Y)*(m_centroid[i].Y - coordinate.Y));
			
            // Compare, if temp is smaller then pre-distance
            if (temp < dis)
			{
                // Insert new distance
				dis = temp;
				coloringindex = i;
			}
		}
	}

    m_coloring[index] = coloringindex;             // Set coloring index
    m_sumcontainer[coloringindex] += coordinate;   // Sum current point to the sumcontainer, this is for avg 
    ++m_totalnumList[coloringindex];               // Sum all points that has same color, this if for avg
}

void KClustering::ClearList(int k)
{
    // Clear and resize the list
    m_sumcontainer.clear();
    m_sumcontainer.resize(k);

    m_totalnumList.clear();
    m_totalnumList.resize(k);
}

bool KClustering::check(void)
{
    int size = int(m_centroid.size());

    int i = 0;

    // Check new centroid and old centorid are same or not 
    for (i = 0; i < size; ++i)
    {
        if (int(m_centroid[i].X*100) == int(m_sumcontainer[i].X * 100) && int(m_centroid[i].Y * 100) == int(m_sumcontainer[i].Y * 100))
            continue;
        else
            break;
    }

    // If there are differnet 
    if (i < size)
    {
        // Insert new centorid and return false
		m_centroid= m_sumcontainer;
        return false;
    }
    // There are same point, end condition, return true
    else
        return true; 
}

void KClustering::sortlist(void)
{
    //Sort the lists 
	std::sort(m_centroid.begin(), m_centroid.end(), compare_coord);
	std::sort(m_sumcontainer.begin(), m_sumcontainer.end(), compare_coord);
}