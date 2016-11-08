#include "FileLoader.hpp"
namespace CS399
{
#define COMMA ','
#define INVALID '?'

  bool FileLoader::LoadCSV(const String & filename)
  {
    Ifstream mfile(filename.c_str(), Ifstream::in);

    if (!mfile.is_open())
    {
      std::cerr << "Fail to open " << filename << std::endl;
      return false;
    }

    bool keySetUp = false;

    // Read file
    while (mfile.good())
    {
      String parser;

      std::getline(mfile, parser);

      if (parser.empty())
        continue;

      size_t comma = parser.find(COMMA);

      // Read a row
      while (comma != String::npos)
      {
        if (!keySetUp)
          mTuple.PushAttribute(parser.substr(0, comma));
        else
          mTuple.PushValue(parser.substr(0, comma));

        parser.erase(0, comma + 1);
        comma = parser.find(COMMA);
      }

      if (!keySetUp)
        mTuple.PushAttribute(parser.substr(0, comma));
      else
        mTuple.PushValue(parser.substr(0, comma));

      // if key has not been set up, Construct it here
      if (!keySetUp)
      {
        mTuple.ConstructAttirbute();
        keySetUp = true;
      }
    }

    mfile.close();
    return true;
  }

  void FileLoader::CleanUp()
  {
    mTuple.Clean();
  }

  void FileLoader::PrintOut(const String & filename)
  {
    Ofstream mOutfile(filename + "_Output.csv", Ofstream::out);

    // print attribute's names
    for (u32 i = 0, size = static_cast<u32>(mTuple.mKeys.size()); i < size; ++i)
    {
      mOutfile << mTuple.mKeys[i];

      if (i + 1 != size)
        mOutfile << ",";
      else
        mOutfile << std::endl;
    }

    // print each tuple line by line
    for (u32 row = 0, elementSize = static_cast<u32>(mTuple.mElements.size()); row < elementSize; ++row)
    {
      for (u32 i = 0, size = static_cast<u32>(mTuple.mElements[row].size()); i < size; ++i)
      {
        mOutfile << mTuple.mElements[row][i];

        if (i + 1 != size)
          mOutfile << ",";
        else
          mOutfile << std::endl;
      }
    }

  }

  void FileLoader::ExtractTupleFromFile(const String & filename, Tuple & tuple)
  {
    Ifstream mfile(filename.c_str(), Ifstream::in);

    if (!mfile.is_open())
    {
      std::cerr << "Fail to open " << filename << std::endl;
      return;
    }

    bool keySetUp = false;

    // Read file
    while (mfile.good())
    {
      String parser;

      std::getline(mfile, parser);

      if (parser.empty())
        continue;

      size_t comma = parser.find(COMMA);

      // Read a row
      while (comma != String::npos)
      {
        if (!keySetUp)
          tuple.PushAttribute(parser.substr(0, comma));
        else
          tuple.PushValue(parser.substr(0, comma));

        parser.erase(0, comma + 1);
        comma = parser.find(COMMA);
      }

      if (!keySetUp)
        tuple.PushAttribute(parser.substr(0, comma));
      else
        tuple.PushValue(parser.substr(0, comma));

      // if key has not been set up, Construct it here
      if (!keySetUp)
      {
        tuple.ConstructAttirbute();
        keySetUp = true;
      }
    }

    mfile.close();
  }

  void FileLoader::ComputeDistance(const Vector2D & table, Vector2D & user, Vector2D & output, Index2D & index)
  {
    u32 usersSize = static_cast<u32>(user.size()); output.resize(usersSize); index.resize(usersSize);
    u32 tableSize = static_cast<u32>(table.size());

    // compute all distances between user and table
    for (u32 userIndex = 0; userIndex < usersSize; ++userIndex)
    {
      output[userIndex].resize(tableSize);

      for (u32 tableIndex = 0; tableIndex < tableSize; ++tableIndex)
      {
        f32 distance = 0.0f;

        for (u32 i = 0, size = static_cast<u32>(table.front().size()); i < size; ++i)
        {
          // Invalid data
          if (user[userIndex][i] < 0.0f)
          {
            index[userIndex].push_back(i);
            user[userIndex][i] = 0.0f;
            continue;
          }

          f32 diff = table[tableIndex][i] - user[userIndex][i];

          distance += (diff * diff);
        }

        // Euclidean distance
        output[userIndex][tableIndex] = sqrtf(distance);
      }
    }
  }

  void FileLoader::ComputeSimilarity(Vector2D & table)
  {
    // compute similiarity
    for (u32 tableIndex = 0, tableSize = static_cast<u32>(table.size()); tableIndex < tableSize; ++tableIndex)
    {
      for (u32 i = 0, size = static_cast<u32>(table.front().size()); i < size; ++i)
        table[tableIndex][i] = 1.0f / (1.0f + table[tableIndex][i]);
    }
  }

  void FileLoader::ComputeTotalScore(const Vector2D & table, const Vector2D & simTable, const Index2D & index, Vector2D & user)
  {
    // calculate total scores for all users. 
    // Because I calculate every user at once, there are nested loops but it is really straight forward way and not inefficient way.
    for (u32 userIndex = 0, usersSize = static_cast<u32>(user.size()), tableSize = static_cast<u32>(table.size()); userIndex < usersSize; ++userIndex)
    {
        for (const auto & i : index[userIndex])
        {
          for (u32 tableIndex = 0; tableIndex < tableSize; ++tableIndex)
            user[userIndex][i] += simTable[userIndex][tableIndex] * table[tableIndex][i];
        }      
    }
  }

  void FileLoader::ComputeEstimatedScore(const Vector2D & simTable, const Index2D & index, Vector2D & user)
  {
    for (u32 userIndex = 0, usersSize = static_cast<u32>(user.size()), tableSize = static_cast<u32>(simTable.size()); userIndex < usersSize; ++userIndex)
    {
      f32 totalSim = 0.0f;
      for (u32 tableIndex = 0; tableIndex < tableSize; ++tableIndex)
        totalSim += simTable[userIndex][tableIndex];

      for (const auto & i : index[userIndex])
        user[userIndex][i] /= totalSim;
    }

  }


  s32 FileLoader::GenerateRand(const s32 min, const s32 max)
  {
    RNG mRNG;
    mRNG.seed(std::random_device()());
    std::uniform_int_distribution<RNG::result_type> randoms(min, max);
    return randoms(mRNG);
  }

#define EXT_CSV ".csv"

  void FileLoader::SolveQuestion(const String & filename, const String & testfile)
  {
    CleanUp();

    // Load and convert values
    this->LoadCSV(filename + EXT_CSV);
    Tuple newUserTable;
    ExtractTupleFromFile(testfile + EXT_CSV, newUserTable);

    PointList mTable, mNewUsers;
    ConvertDataInto2D(mTable, this->mTuple);
    ConvertDataInto2D(mNewUsers, newUserTable);
    
    Index2D indexList;
    Vector2D simTable;

    Ofstream mOutfile(filename + "_Output.csv", Ofstream::out);

    // compute distance
    ComputeDistance(mTable, mNewUsers, simTable, indexList);

    mOutfile << "# New user's distance " << std::endl;
    PrintOut(mNewUsers, mOutfile);
    
    // coumpute similarities
    ComputeSimilarity(simTable);

    mOutfile << std::endl << "# Similarity " << std::endl;
    PrintOut(simTable, mOutfile);

    // compute total score
    ComputeTotalScore(mTable, simTable, indexList, mNewUsers);
    

    // compute estimated score
    ComputeEstimatedScore(simTable, indexList, mNewUsers);


    mOutfile << std::endl << "# Recomended " << std::endl;
    PrintOut(mNewUsers, mOutfile);
  }

#undef COMMA
#undef INVALID
#undef EXT_CSV

  void FileLoader::Tuple::PreMakeTuple()
  {
    mElements.push_back(std::vector<String>());
    mElementIter = mElements.end() - 1;
  }

  void FileLoader::Tuple::PushAttribute(const String & attribute)
  {
    mKeyQueue.push(attribute);
  }

  void FileLoader::Tuple::PushValue(const String & element)
  {
    if (mElementIter->size() == mKeys.size())
    {
      PreMakeTuple();
    }

    mElementIter->push_back(element);
  }

  void FileLoader::Tuple::ConstructAttirbute()
  {
    mKeys.reserve(mKeyQueue.size());

    // Copy
    while (!mKeyQueue.empty())
    {
      mKeys.push_back(mKeyQueue.front());
      mKeyQueue.pop();
    }

    mElements.reserve(0x010); // premake 0x10 size
    PreMakeTuple();
  }



  void FileLoader::ConvertDataInto2D(Vector2D & vec, const Tuple & tuples)
  {
    vec.resize(tuples.mElements.size());

    for (u32 i = 0, size = static_cast<u32>(tuples.mElements.size()); i < size; ++i)
    {
      u32 elements_size = static_cast<u32>(tuples.mElements[i].size());
      vec[i].resize(elements_size - 1);
      for (u32 index = 1; index < elements_size; ++index)
        vec[i][index - 1] = std::stof(tuples.mElements[i][index]);
    }

  }


  void FileLoader::PrintOut(const String & filename, const PointList & centroids, MarkerList & output)
  {
    Ofstream mOutfile(filename + "_Output.csv", Ofstream::out);

    //mOutfile << "#Line cosits of " << std::endl << "#Cluster Name, " << "Number of instances, " << " Mean of " << mTuple.mKeys[1] << ", Mean of " << mTuple.mKeys[2] << std::endl <<
    //  "#Subs consist of " << ", " << mTuple.mKeys[0] << ", " << mTuple.mKeys[1] << ", " << mTuple.mKeys[2] << std::endl;
    //std::vector<u32> outputSize;
    //
    //for (u32 i = 0, size = static_cast<u32>(centroids.size()); i < size; ++i)
    //{
    //  outputSize.push_back(static_cast<u32>(output[i].size()));
    //  mOutfile << "Cetroid" << i << "," << output[i].size() << "instances," << centroids[i].x << "," << centroids[i].y << std::endl;
    //  while (!output[i].empty())
    //  {
    //    const auto & instance = mTuple.mElements[output[i].top()];
    //    mOutfile << instance[0] << "," << instance[1] << "," << instance[2] << std::endl;
    //    output[i].pop();
    //  }
    //}
    //
    //mOutfile << std::endl;

    //// Print percentage at the end of line
    //for (u32 i = 0, size = static_cast<u32>(outputSize.size()); i < size; ++i)
    //  mOutfile << "#Cetroid" << i << ", " << outputSize[i] << ", ("
    //  << (outputSize[i] / static_cast<f32>(mTuple.mElements.size())) * 100.0f
    //  << "%)" << std::endl;

    mOutfile.close();
  }

  void FileLoader::PrintOut(const Vector2D & table, Ofstream & out)
  {
    for (u32 i = 0, size = static_cast<u32>(mTuple.mKeys.size()); i < size; ++i)
    {
      out << mTuple.mKeys[i];
      if (i + 1 != size)
        out << ", ";
    }
    
    out << std::endl;

    for (u32 userIndex = 0, usersSize = static_cast<u32>(table.size()), tableSize = static_cast<u32>(table.front().size()); userIndex < usersSize; ++userIndex)
    {
      out << "U" << userIndex << ", ";
      for (u32 i = 0; i < tableSize; ++i)
      {
        out << table[userIndex][i];
        if (i + 1 != tableSize) out << ", ";
      }
      out << std::endl;
    }
  }

  void FileLoader::PrintSimilarity(const Vector2D & table, Ofstream & out)
  {

  }

}
