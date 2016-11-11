#include "def.h"
#include "KClustering.h"
#include <cstdio>
DATASET TShirt_DATA;

bool ReadData(void);
void test_clustring(KClustering* kmean, int k, int testid);
void dum_test_clustring(KClustering* kmean);

///////////////////////////////////////////////////////////////////
// Test set
void testk2(KClustering* kmean)
{
	test_clustring(kmean, 2, 1);
}

void testk3(KClustering* kmean)
{
	test_clustring(kmean, 3, 2);
}

void testk4(KClustering* kmean)
{
	test_clustring(kmean, 4, 3);
}

void testk5(KClustering* kmean)
{
	test_clustring(kmean, 5, 4);
}

void testk10(KClustering* kmean)
{
	test_clustring(kmean, 10, 5);
}
///////////////////////////////////////////////////////////////////


int main(void)
{
    // Read data from the file
	ReadData();          

    // Create clustering object
	KClustering *kmean = new KClustering();

    // Test set 
	testk2(kmean);  // test k=2
	                
    testk3(kmean);  // test k=3
	                
    testk4(kmean);  // test k=4
	                
    testk5(kmean);  // test k=5
	
    testk10(kmean); // test k=10
	
    dum_test_clustring(kmean); // dummy test with k=2

    // delete object
	delete kmean;

	return 0;
}

void dum_test_clustring(KClustering* kmean)
{
	int k = 3;

    // Do clustering test 100 times
	for (int i = 0; i < 100; ++i)
	{
        // Clustering
		std::vector<XYcoord> result = kmean->DoClustering(&TShirt_DATA, k);

        // Print 
        std::cout << "Test" << i + 1 << "\n" << "K = " << k << "\n";
		for (int i = 0; i < result.size(); ++i)
		{
            std::cout << "Centroid " << i + 1 << ": (" << result[i].X << " , " << result[i].Y << ")\n";
		}
        std::cout << std::endl;
	}
}
void test_clustring(KClustering* kmean, int k, int testid)
{
    // Do clustering test 5 times
	for (int i = 0; i < 5; ++i)
	{
        // Clustering
		std::vector<XYcoord> result = kmean->DoClustering(&TShirt_DATA, k);

        // Print 
		std::cout << "Test" << i+1 << "\n" << "K = " << k << "\n";
		for (int i = 0; i < result.size(); ++i)
		{
            std::cout << "Centroid " << i + 1 << ": (" << result[i].X << " , " << result[i].Y << ")\n";
		}
        std::cout << std::endl;
	}
}
bool ReadData(void)
{
	//Open the file
	CsvParser *csvparser = CsvParser_new("Tshirt Data.csv", ",", 1);
	CsvRow *header;
	CsvRow *row;

	//Header of this file
	header = CsvParser_getHeader(csvparser);
	int attNum = 0;

	if (header == NULL)
	{
		printf("%s\n", CsvParser_getErrorMessage(csvparser));
		return 1;
	}
	else
	{
		attNum = CsvParser_getNumFields(header);
		const char **rowFields = CsvParser_getFields(header);
		TShirt_DATA.resize(attNum);
		for (int i = 0; i < attNum; ++i)
			TShirt_DATA[i].push_back(rowFields[i]);
	}

	//Get data and store it in array
	unsigned index = 0;
	while ((row = CsvParser_getRow(csvparser)))
	{
		const char **rowFields = CsvParser_getFields(row);

		for (int i = 0; i < attNum; ++i)
			TShirt_DATA[i].push_back(rowFields[i]);
		++index;
	}

	CsvParser_destroy(csvparser);

	return true;
}

