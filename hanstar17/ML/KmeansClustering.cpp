#include "KMeansClustering.h"
#include "Dataframe.h"
#include "RandomSampler.h"

static double Sq(double v)
{
	return v*v;
}

static double DistSq(const DataPoint& p1, const DataPoint& p2)
{
	return Sq(p1.X - p2.X) + Sq(p1.Y - p2.Y);
}

KMeansClustering::KMeansClustering(Dataframe& dataframe, const std::string& attX, const std::string& attY)
	: _dataframe(dataframe)
	, _attX(attX)
	, _attY(attY)
	, _o(nullptr)
{
}

void KMeansClustering::SetDebugOutput(std::ofstream* o)
{
	_o = o;
}

const std::vector<const Instance*>& KMeansClustering::Get(int i) const
{
	return _clusters[i];
}

std::vector<DataPoint> KMeansClustering::SampleDataPoints(const int n)
{
	std::vector<DataPoint> points;
	points.reserve(n);

	// sample n instances
	RandomSampler sampler;
	sampler.Sample(_dataframe, n);

	// push them into output container
	const std::vector<const Instance*>& samples = sampler.GetSamples();
	for (const auto& sample : samples)
	{
		double x = _dataframe.GetInstanceAttribute(sample, _attX).AsDouble();
		double y = _dataframe.GetInstanceAttribute(sample, _attY).AsDouble();
		points.push_back({ x, y });
	}

	return std::move(points);
}

DataPoint KMeansClustering::ToDataPoint(const Instance* instance)
{
	DataPoint point;
	point.X = _dataframe.GetInstanceAttribute(instance, _attX).AsDouble();
	point.Y = _dataframe.GetInstanceAttribute(instance, _attY).AsDouble();
	return std::move(point);
}

DataPoint KMeansClustering::CalculateCentroid(const std::vector<const Instance*>& instances)
{
	DataPoint centroid = { 0.0, 0.0 };

	// sums all the given points
	for (auto& instance : instances)
	{
		DataPoint point = ToDataPoint(instance);
		centroid.X += point.X;
		centroid.Y += point.Y;
	}

	// divide the sum by the number of instances
	centroid.X /= instances.size();
	centroid.Y /= instances.size();

	// return it
	return std::move(centroid);
}

void KMeansClustering::PrintClusters(std::ofstream& o)
{
	for (int i = 0; i < int(_clusters.size()); ++i)
	{
		DataPoint centroid = CalculateCentroid(_clusters[i]);
		PrintClusters(o, i + 1, centroid);
	}
}

void KMeansClustering::PrintClusters(std::ofstream& o, const int group, const DataPoint& centroid)
{
	// print group info
	o << "Group " << group << " :" << "n=" << _clusters[group - 1].size() << ", " << "centroid=(" << centroid.X << ", " << centroid.Y << ")" << std::endl;
	o << "---------------------" << std::endl;
	// print info of instances
	for (auto& instance : _clusters[group - 1])
	{
		o << instance->GetAttribute(0).AsString() << " : " <<
			_attX << "=" << _dataframe.GetInstanceAttribute(instance, _attX).AsString() << ", " <<
			_attY << "=" << _dataframe.GetInstanceAttribute(instance, _attY).AsString() << "" << std::endl;
	}
	o << std::endl;
}

void KMeansClustering::Cluster(const int k)
{
	// randomly select k points from instances
	std::vector<DataPoint> centroids = std::move(SampleDataPoints(k));

	// prints initial centroids
	if (_o)
	{
		for (int i = 0; i < k; ++i)
			(*_o) << "Initial centroid for group " << i+1 << " : " << centroids[i].X << ", " << centroids[i].Y << std::endl;

		(*_o) << std::endl;
	}
	
	// prepare k clusters
	_clusters.clear();
	_clusters.resize(k);

	int iteration = 0;
	bool centroidsChanged = true;
	double sumDist;
	while (centroidsChanged) // if centroid is not changed, exit loop
	{
		// clear up clusters and sumDist to regroup instances
		for (auto& cluster : _clusters)
			cluster.clear();
		sumDist = 0.0;

		// assign each instance to a cluster whose centroid is closest to it
		auto& instances = _dataframe.GetInstances();
		for (auto& instance : instances)
		{
			DataPoint point = ToDataPoint(instance);

			// find the closest cluster-group
			double minDistSq = std::numeric_limits<double>::max();
			int minDistCluster = 0;
			for (int i = 0; i < k; ++i)
			{
				double distSq = DistSq(centroids[i], point);
				if (distSq < minDistSq)
				{
					minDistSq = distSq;
					minDistCluster = i;
				}
			}

			// add instance to the group
			_clusters[minDistCluster].emplace_back(instance);
			if (_o)
				sumDist += sqrtl(minDistSq);
		}

		// debug output : iteration info
		if (_o)
		{
			(*_o) << "-----------------------" << std::endl;
			(*_o) << "iteration " << iteration << std::endl << std::endl;
		}

		// update centroids of clusters with their elements
		centroidsChanged = false;
		for (int i = 0; i < k; ++i)
		{
			DataPoint centroid = CalculateCentroid(_clusters[i]);
			if (centroid.X != centroids[i].X || centroid.Y != centroids[i].Y)
				centroidsChanged = true;

			centroids[i] = centroid;

			if (_o)
				PrintClusters(*_o, i+1, centroid);
		}
		if (_o)
			(*_o) << "Sum Distance : " << sumDist << std::endl;

		++iteration;
	}
}
