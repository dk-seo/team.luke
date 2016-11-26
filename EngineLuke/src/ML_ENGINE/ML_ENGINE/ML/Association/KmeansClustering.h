#pragma once
#include <vector>
#include <fstream>

class Dataframe;
class Instance;

template <typename T = double>
struct DataPointTemplate
{
  std::vector<T> mDataPoints;
};

// double type's data point
typedef DataPointTemplate<>    DataPoint;
typedef std::vector<DataPoint> ClusterData;

class KMeansClustering
{
public:
	// requires dataframe to cluster + ignored attribute names
	KMeansClustering(Dataframe& dataframe,
    std::vector<std::string> & ignores = std::vector<std::string>());

	// if ofstream is not null then prints all the course of works while clustering to the stream.
	void SetDebugOutput(std::ofstream* o);

	// cluster into k groups
  ClusterData Cluster(const int k);

	// get a specific cluster group of index
	const std::vector<const Instance*>& Get(int i) const;

	// print clustered groups and its members to given stream
	void PrintClusters(std::ofstream& o);

  // convert instance to datapoint
  DataPoint ToDataPoint(const Instance* instance);

private:
	// sample n datapoints from dataframe
	std::vector<DataPoint> SampleDataPoints(const int n);

	// calculate centroids with given instances
	DataPoint CalculateCentroid(const std::vector<const Instance*>& instances);

	// print clusters with given stream, cluster-group, and centroid
	void PrintClusters(std::ofstream& o, const int group, const DataPoint& centroid);

  // calculate N dimensions Square by Euclidean 
  double DistSq(const DataPoint & p1, DataPoint & p2);

public:
  // dot product if two data have differnet size return 0.0
  static double Dot(const DataPoint & p1, const DataPoint & p2,
    const std::vector<int> ignores = std::vector<int>());

  // length
  static double Length(const DataPoint & p,
    const std::vector<int> ignores = std::vector<int>(),
    const bool Sqrt = false);

private:
	Dataframe& _dataframe; // dataframe to be used
  std::vector<int> _ignores; // ignore field's indices
	std::vector<std::vector<const Instance*>> _clusters; // cluster-groups with instances
	std::ofstream* _o; // output stream for debug
};

