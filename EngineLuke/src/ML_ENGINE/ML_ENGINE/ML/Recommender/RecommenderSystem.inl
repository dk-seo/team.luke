/******************************************************************************/
/*!
\file RecommenderSystem.inl
\project CS399_TeamLuke
\author Deok-Hwa (DK) Seo

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/

template<typename Cluster, typename Data>
inline Recommender<Cluster, Data>::Recommender(Dataframe & data) :
  mTable(data), mCluster(data),
  mIgnores(), pOStream(nullptr),
  mClusterGroup(1), mPrecision(0.5f),
  mFlag(DO_CLUSTER | DO_RECOMMEND)
{
}

template<typename Cluster, typename Data>
inline Recommender<Cluster, Data>::~Recommender()
{
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::ChangeDataFrame(Dataframe & data)
{
  mTable = data;
  mClusterUpdatable = true;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetDebugOutput(std::ostream * o)
{
  pOStream = o;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetGroupNumber(const int k)
{
  if (k < 1) return;
  
  mClusterGroup = k;
  mFlag |= DO_CLUSTER;
}

template<typename Cluster, typename Data>
inline int Recommender<Cluster, Data>::GetGroupNumber() const
{
  return mClusterGroup;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetPrecision(const float percent)
{
  if (percent <= 0.0f) return;
  
  mPrecision = percent;
  mFlag |= DO_RECOMMEND;
}

template<typename Cluster, typename Data>
inline float Recommender<Cluster, Data>::GetPrecision() const
{
  return mPrecision;
}

template<typename Cluster, typename Data>
inline Answers & Recommender<Cluster, Data>::Recommend(IndexList & favorList)
{
  if (mFlag & DO_CLUSTER) mFlag |= DO_RECOMMEND;
  if ((mFlag & DO_RECOMMEND) == 0x00) return mRecommendList;

  // calculate cluster
  if(mFlag & DO_CLUSTER)
    clustered = mCluster.Cluster(mClusterGroup);

  Answers mRecommends;

  IndexList ignores(mIgnores.size());

  // put ignore attribute's indices
  for (auto & i : mIgnores)
    ignores.push_back(mTable.GetAttributeIndex(i));

  std::vector<DataPoint> dataTable;
  mRecommends.resize(mClusterGroup);
  {
    std::vector<double> cluster_len;

    // assign each instance to a cluster whose centroid is closest to it
    std::vector<Instance*> instances = mTable.GetInstances();
    dataTable.resize(instances.size());

    for (unsigned index = 0; index < instances.size(); ++index)
    {
      DataPoint point = mCluster.ToDataPoint(instances[index]);
      double point_len = KMeansClustering::Length(point, ignores);

      // calculate cosine similarity on N dimensions 
      for (int i = 0, size = static_cast<int>(clustered.size());
        i < size; ++i)
      {
        const auto & cluster = clustered[i];
        if (cluster_len.size() < size)
          cluster_len.push_back(KMeansClustering::Length(cluster, ignores));
        double similarity = sqrtl(point_len * cluster_len[i]);
        similarity = KMeansClustering::Dot(cluster, point, ignores) / similarity;

        // pick when the similarity is greater than or equal to a specific precision
        if (similarity >= mPrecision)
          mRecommends[i].emplace_back(index, similarity);
      }
      dataTable[index] = std::move(point);
    }
  }

  double grade = 0.0;
  Answer highest;
  int quality = -1;
  {
    auto qualityIter = std::find(mIgnores.begin(), mIgnores.end(), "quality");
    if (qualityIter != mIgnores.end())
      quality = mTable.GetAttributeIndex(*qualityIter);
  }
  
  mRecommendList.resize(mRecommends.size());
  
  // find the highest quality 
  if (quality >= 0)
  {
    for (int index = 0; index < static_cast<int>(mRecommends.size()); ++index)
    {
      const auto recommenderList = mRecommends[index];
      for (int i = 0; i < recommenderList.size(); ++i)
      {
        auto & obj = recommenderList[i];
        if (grade < dataTable[obj.first].mDataPoints[quality])
        {
          grade = dataTable[obj.first].mDataPoints[quality];
          highest.clear();
        }
        else if (grade == dataTable[obj.first].mDataPoints[quality])
          highest.emplace_back(obj.first, obj.second);
      }
      mRecommendList[index] = std::move(highest);
    }
  }

  
  mFlag = 0;
  return mRecommendList;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::AddIgnoreAttribute(std::string & attribute)
{
  auto result = std::find(mIgnores.begin(), mIgnores.end(), attribute);
  if (result == mIgnores.end())
  {
    mIgnores.emplace_back(attribute);
    mCluster.AddIgnore(attribute);
  }
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::RemoveIgnoreAttribute(std::string & attribute)
{
  auto result = std::find(mIgnores.begin(), mIgnores.end(), attribute);
  if (result != mIgnores.end())
  {
    mIgnores.erase(result);
    mCluster.RemoveIgnore(attribute);
  }
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::DisplayIgnoreAttribute(std::ostream * o)
{
  if (o == nullptr) return;

  (*o) << "----------------------------" << std::endl
  << "Attributes on ignore list :" << std::endl;

  for (const auto & i : mIgnores)
    o << i << std::endl;

  (*o) << "----------------------------" << std::endl;
}

template<typename Cluster, typename Data>
inline bool Recommender<Cluster, Data>::IsInIgnoreList(const std::string & attribute)
{
  auto result = std::find(mIgnores.begin(), mIgnores.end(), attribute);
  return (result != mIgnores.end());
}

template<typename Cluster, typename Data>
inline std::vector<double>
  Recommender<Cluster, Data>::GetFeatureVector(Instance * instance,
    const std::vector<int>& indices)
{
  std::vector<double> featureVector;
  featureVector.reserve(indices.size());
  for (size_t i = 0; i < indices.size(); ++i)
  {
    featureVector.emplace_back(
      instance->GetAttribute(indices[i]).AsDouble());
  }

  return std::move(featureVector);
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::PrintAttributes(const std::vector<int>& attIndices)
{
  if (pOStream == nullptr) return;

  for (size_t i = 0; i < attIndices.size(); ++i)
  {
    if (i > 0)
      *pOStream << ", ";
    *pOStream << mTable.GetAttributeName(attIndices[i]);
  }
}
