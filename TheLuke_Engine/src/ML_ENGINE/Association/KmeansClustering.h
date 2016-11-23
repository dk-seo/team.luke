#pragma once
#include <vector>
#include <fstream>

class Dataframe;
class Instance;

struct DataPoint
{
	double X;
	double Y;
};

class KMeansClustering
{
public:
	// requires dataframe to cluster, attribute names for x axis and y axis
	KMeansClustering(Dataframe& datframe, const std::string& attX, const std::string& attY);

	// if ofstream is not null then prints all the course of works while clustering to the stream.
	void SetDebugOutput(std::ofstream* o);

	// cluster into k groups
	void Cluster(const int k);

	// get a specific cluster group of index
	const std::vector<const Instance*>& Get(int i) const;

	// print clustered groups and its members to given stream
	void PrintClusters(std::ofstream& o);

private:
	// sample n datapoints from dataframe
	std::vector<DataPoint> SampleDataPoints(const int n);

	// convert instance to datapoint
	DataPoint ToDataPoint(const Instance* instance);

	// calculate centroids with given instances
	DataPoint CalculateCentroid(const std::vector<const Instance*>& instances);

	// print clusters with given stream, cluster-group, and centroid
	void PrintClusters(std::ofstream& o, const int group, const DataPoint& centroid);

private:
	Dataframe& _dataframe; // dataframe to be used
	std::string _attX; // attribute name for x axis
	std::string _attY; // attribute name for y axis
	std::vector<std::vector<const Instance*>> _clusters; // cluster-groups with instances
	std::ofstream* _o; // output stream for debug
};
