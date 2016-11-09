#include "KMeansClusteringTest.h"
#include <iostream>
#include <fstream>
#include <random>
#include "Dataframe.h"
#include "KmeansClustering.h"


// This function returns a random point whose x and y range from 0 to 1
static std::pair<double, double> RandomPoint()
{
	static std::random_device rd;
	static std::default_random_engine randomGenerator(rd());
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);

	double x = distribution(randomGenerator);
	double y = distribution(randomGenerator);
	return std::move(std::pair<double, double>(x, y));
}

// creates a csv file that has randomly generated x and y that range from 0 to 1
static bool CreateRandomXYDataCsv(const std::string& filename, int n)
{
	std::ofstream in(filename, std::ios::out);
	if (!in)
		return false;

	in.precision(2);
	in << std::fixed;

	// print csv header
	in << "ID,X,Y" << std::endl;

	// print randomly generated points
	for (int i = 0; i < n; ++i)
	{
		std::pair<double, double>&& point = RandomPoint();
		in << i << "," << point.first << "," << point.second << std::endl;
	}
	in.close();
	return true;
}

// Test function for KMeansClustering
static void KMeansClusterTest(int k, // number of groups to be clustered
	const std::string& inputfile,
	const std::string& outputile,
	const std::string& attX, // attribute name used for x-axis
	const std::string& attY, // attribute name used for y-axis
	bool debugOutput = false) // weather to print debug output or not
{
	// build dataframe from input csv file
	Dataframe dataframe;
	if (!dataframe.BuildFromCsv(inputfile, true))
	{
		std::cout << dataframe.GetErrorMessage() << std::endl;
		return;
	}

	// create/open output file
	std::ofstream out(outputile, std::ios::out);
	if (!out)
	{
		std::cout << outputile << " file creation failed!" << std::endl;
		return;
	}

	out.precision(2);
	out << std::fixed;

	// cluster with dataframe
	KMeansClustering clustering(dataframe, attX, attY);
	if (debugOutput)
		clustering.SetDebugOutput(&out);

	clustering.Cluster(k);

	// print results
	out << std::endl << std::endl;
	out << "----------------------" << std::endl;
	out << "Final Results: " << std::endl;
	out << "----------------------" << std::endl << std::endl;
	clustering.PrintClusters(out);
	out << std::endl << "Done." << std::endl;

	// close outputfile
	out.close();
}

void KMeanClustering_RandomXY()
{
	static const bool g_CreateRandomXYData = false;

	// creates random xy data csv file.
	const char* inputfile = "RandomXYData.csv";
	if (g_CreateRandomXYData && !CreateRandomXYDataCsv(inputfile, 20))
	{
		std::cout << inputfile << " file creation failed!" << std::endl;
		return;
	}

	// cluster test
	KMeansClusterTest(2, inputfile, "Q1_XY.txt", "X", "Y");
	KMeansClusterTest(2, inputfile, "Q1_XY_DEBUG.txt", "X", "Y", true);
}

void KMeanClustering_ShirtsData()
{
	// cluster test
	KMeansClusterTest(3, "TShirt Data.csv", "Q1_ShirtsData.txt", "height (inches)", "weight (pounds)");
	KMeansClusterTest(3, "TShirt Data.csv", "Q1_ShirtsData_DEBUG.txt", "height (inches)", "weight (pounds)", true);
}