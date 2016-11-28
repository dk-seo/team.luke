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
  mClusterGroup(1)
{
}

template<typename Cluster, typename Data>
inline Recommender<Cluster, Data>::~Recommender()
{
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetDebugOutput(std::ostream * o)
{
  pOStream = o;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetGroupNumber(const int k)
{
  if(k >= 1) mClusterGroup = k;
}

template<typename Cluster, typename Data>
inline int Recommender<Cluster, Data>::GetGroupNumber() const
{
  return mClusterGroup;
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::SetPrecision(const float percent)
{
  if (percent >= 0.0f) mPrecision = percent;
}

template<typename Cluster, typename Data>
inline float Recommender<Cluster, Data>::GetPrecision() const
{
  return mPrecision;
}

template<typename Cluster, typename Data>
inline IndexList Recommender<Cluster, Data>::Recommend(IndexList & favorList)
{
  IndexList mOut;

  mCluster.Cluster(mClusterGroup);

  return std::move(mOut);
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::AddIgnoreAttribute(const std::string & attribute)
{
  auto result = std::find(mIgnores.begin(), mIgnores.end(), attribute);
  if (result == mIgnores.end())
    mIgnores.emplace_back(attribute);
}

template<typename Cluster, typename Data>
inline void Recommender<Cluster, Data>::RemoveIgnoreAttribute(const std::string & attribute)
{
  auto result = std::find(mIgnores.begin(), mIgnores.end(), attribute);
  if (result != mIgnores.end())
    mIgnores.erase(result);
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
