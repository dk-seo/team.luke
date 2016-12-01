/******************************************************************************/
/*!
\file KmeansClustering.h
\project CS399_TeamLuke
\author Hanbyul Jeon, Deok-Hwa (DK) Seo

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
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

// comparison functor
static int compareMyType(const void * a, const void * b)
{
  if (*(double*)a < *(double*)b) return -1; else
    if (*(double*)a == *(double*)b) return  0;
  return 1; // a > b
}

// K-Means Clustering
class KMeansClustering
{
public:
  // requires dataframe to cluster + ignored attribute names
  KMeansClustering(Dataframe& dataframe,
    std::vector<std::string> & ignores = std::vector<std::string>());

  // add ignores
  void AddIgnore(std::string & ignores);

  // remove ignores
  void RemoveIgnore(std::string & ignores);

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

  // get min/max of each attribute
  const std::vector<std::pair<double, double>> &
    GetLimits() const;

  // get difference between min and max of each attribute
  const std::vector<double> & GetDiff() const;

private:
  // sample n datapoints from dataframe
  std::vector<DataPoint> SampleDataPoints(const int n);

  // calculate centroids with given instances
  DataPoint CalculateCentroid(const std::vector<const Instance*>& instances);

  // print clusters with given stream, cluster-group, and centroid
  void PrintClusters(std::ofstream& o, const int group, const DataPoint& centroid);

  // calculate N dimensions Square by Euclidean 
  double DistSq(const DataPoint & p1, DataPoint & p2);

  // calculate min/max limit on each attribute
  void CalculateLimits();

public:
  // dot product if two data have differnet size return 0.0
  static double Dot(const DataPoint & p1, const DataPoint & p2,
    const std::vector<int> & ignores = std::vector<int>(),
    const std::vector<double> & diffs = std::vector<double>());

  // length
  static double Length(const DataPoint & p,
    const std::vector<int> & ignores = std::vector<int>(),
    const std::vector<double> & diffs = std::vector<double>(),
    const bool Sqrt = false);

private:
  Dataframe& _dataframe; // dataframe to be used
  std::vector<int> _ignores; // ignore field's indices
  std::vector<std::vector<const Instance*>> _clusters; // cluster-groups with instances
  std::ofstream* _o; // output stream for debug
  std::vector<std::pair<double, double>> _limits; // limit values
  std::vector<double> _diffs; // diffs storage
};

