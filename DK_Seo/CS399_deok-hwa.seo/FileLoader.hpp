#ifndef DK_FILE_LOADER_HPP
#define DK_FILE_LOADER_HPP

#include "Typedef.hpp" // pre-included standard header files

namespace CS399
{ 
  class FileLoader
  {
  public:
    FileLoader() {}

  private:
    // Load CSV
    bool LoadCSV(const String & filename);

    // Clean up data
    void CleanUp();

    // Print out to CSV
    void PrintOut(const String & filename);

    // Slowly generate random number
    s32 GenerateRand(const s32 min, const s32 max);

  public:
    void SolveQuestion(const String & filename, const String & testfile = "newUser");

  private:
    typedef std::pair<String, f32> ValuePair; // Value pair

    // Comparison struct between two value pairs
    struct ValueComp
    {
      bool operator()(const ValuePair& lhs, const ValuePair& rhs) {
        return lhs.second < rhs.second;
      }
    };

    typedef std::map<String, f32>                ValueSet; // Set of value pair
    typedef std::unordered_map<String, ValueSet> KeyValue; // Map with Key and Value Set
    
    // 2D Vector type
    typedef std::vector<std::vector<f32>> Vector2D;

    // 2D Index type
    typedef std::vector<std::vector<u32>>  Index2D;


    //Custom tuple
    class Tuple
    {
    private:
      std::queue<String> mKeyQueue; // temporarily queueing the keys

    public:
      std::vector<String> mKeys;
      std::vector<std::vector<String>> mElements;
      std::vector<std::vector<String>>::iterator mElementIter;
      
    private:
      void PreMakeTuple(); // premake one extra tuple

    public:
      void PushAttribute(const String & attribute); // Push attribute
      void PushValue(const String & element); // Push stack
      void ConstructAttirbute(); // Construct attribute. This should be called only once.
      
      inline bool IsAttributeSetUp() { return (mKeys.size() != 0); }
      inline void Clean()
      {
        mKeys.clear();
        mElements.clear();
        mElementIter = mElements.end();
      }
    };

  private:
    Tuple mTuple;

  private:
    // Convert Data into 2D 
    void ConvertDataInto2D(Vector2D & vec, const Tuple & tuples);

    // Extract Tuple variables
    void ExtractTupleFromFile(const String & filename, Tuple & tuple);

    // Compute Euclidean Distance
    void ComputeDistance(const Vector2D & table, Vector2D & user, Vector2D & output, Index2D & index);

    // Compute Similarity
    void ComputeSimilarity(Vector2D & table);

    // Compute Total Score
    void ComputeTotalScore(const Vector2D & table, const Vector2D & simTable, const Index2D & index, Vector2D & user);

    // Compute Estimated Score
    void ComputeEstimatedScore(const Vector2D & simTable, const Index2D & index, Vector2D & user);

    // Compare method for greater 
    struct greater {
      bool operator()(const s32 lhs, const s32 rhs) { return lhs > rhs; };
    };

    // To label objects and cache friendly read (in case of thounds data)
    typedef std::priority_queue<u32, std::vector<u32>, greater> Marker;
    typedef std::vector<Marker> MarkerList;
    typedef Vector2D PointList;

    void PrintOut(const String & filename, const PointList & centroids, MarkerList & output);
    void PrintOut(const Vector2D & table, Ofstream & out);
    void PrintSimilarity(const Vector2D & table, Ofstream & out);
  };

#include "FileLoader.inl"
}

#endif //fileloader.hpp