/******************************************************************************/
/*!
\file UserInterface.cpp
\project CS399_TeamLuke
\author Jiyun Kang

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "UserInterface.h"
#include "FileIO\FileSystem.h"
#include "UI\Imgui\imconfig.h"
#include "UI\Q3Window.h"
#include "ML\Utilities\Statistics.h"
//For Q3
#include "ML\Regression\Equation.h"
#include "ML\Regression\LinearRegression.h"
#include "Test\LinearRegressionTest.h"
#include "ML/Recommender/RecommenderSystem.h" // recommender


struct UI::pImpl {
  WineRecommender* pRecommender;
};

UI* UI::s_pinstance = NULL;

UI::UI(void)
  :m_dataframe(nullptr),
  mRecommender(std::make_unique<pImpl>()),
  mUpdatable(true)
{
}
UI::~UI(void)
{
  if (mRecommender->pRecommender)
  {
    delete mRecommender->pRecommender;
    mRecommender->pRecommender = nullptr;
  }

  if (m_dataframe)
    delete m_dataframe;
}

void UI::Initialize(void)
{
  m_dataframe = nullptr;
}

void UI::Update(void)
{
  MainWindow();

  //UpdateMethodWindow();
  UpdateQuestionsWindow();

}


void UI::Shutdown(void)
{

}

void UI::MainWindow(void)
{
  ImGui::SetNextWindowPos(ImVec2(15, 50), ImGuiSetCond_Once);
  ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiSetCond_Once);

  if (ImGui::Begin("Preprocess"))
  {
    LoadCSVfile();

    ImGui::Columns(2, "mycolumns");

    if (ImGui::CollapsingHeader("Current relation", (const char*)0, true, true))
    {
      ShowRelation();
    }

    if (ImGui::CollapsingHeader("Attributes", (const char*)0, true, true))
    {
      ShowAttributes();
    }

    ImGui::NextColumn();

    if (ImGui::CollapsingHeader("Selected Attributes", (const char*)0, true, true))
    {
      ShowSeleAttr();
    }
    //if (ImGui::CollapsingHeader("Plot", (const char*)0, true, true))
    {
      //PlotHistogram();
    }
    ImGui::End();
  }
}

void UI::ShowErrorWindow(std::string message, bool* open)
{
  if (ImGui::Begin("Error", open, ImGuiWindowFlags_MenuBar))
  {
    ImGui::Text(message.c_str());

    ImGui::End();
  }
}

void UI::UpdateMethodWindow(void)
{
  ImGui::SetNextWindowPos(ImVec2(930, 130), ImGuiSetCond_Once);
  ImGui::SetNextWindowSize(ImVec2(300, 70), ImGuiSetCond_Once);

  if (!curr_filepath.empty())
  {
    if (ImGui::Begin("Methods"))
    {
      if (ImGui::Button("Classify"))
      {

      }
      ImGui::SameLine();
      if (ImGui::Button("Associate"))
      {

      }
      ImGui::SameLine();
      if (ImGui::Button("Cluster"))
      {

      }
      ImGui::End();
    }
  }
}

void UI::UpdateQuestionsWindow(void)
{
  ImGui::SetNextWindowPos(ImVec2(930, 50), ImGuiSetCond_Once);
  ImGui::SetNextWindowSize(ImVec2(300, 70), ImGuiSetCond_Once);
  if (!curr_filepath.empty())
  {
    if (ImGui::Begin("Questions"))
    {
      Q1Q2();

      ImGui::SameLine();

      Q3();

      ImGui::SameLine();

      RecommenderSystem();

      ImGui::End();
    }
  }
}

void UI::LoadCSVfile(void)
{
  static bool loadWindow = false;
  if (ImGui::Button("Choose"))
    loadWindow = true;

  ImGui::SameLine();
  ImGui::Text(curr_filepath.c_str());

  ImGui::Separator();

  if (!loadWindow)
    return;

  if (loadWindow)
  {
    ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_Always);
    if (ImGui::Begin(" File Load ", &loadWindow, ImVec2(0, 0)))
    {
      std::vector<std::string> fileList = GetFilesNameFromDirectory(std::string("Data"));
      static int currIndex = -1;

      ImGui::Combo(" Data File ", &currIndex, StringItemsGetter, &fileList, int(fileList.size()));

      if (ImGui::Button(" Load "))
      {
        if (currIndex != -1)
        {
          if (curr_filepath != fileList[currIndex])
          {
            mUpdatable = true;
            curr_filepath = fileList[currIndex];
          }

          std::string path = "Data/" + curr_filepath;


          if (m_dataframe)
          {
            delete m_dataframe;
            m_dataframe = nullptr;
          }

          m_dataframe = new Dataframe;
          mUpdatable = true;

          if (!m_dataframe->BuildFromCsv(path, true))
          {
            bool open = true;
            UI::Get().ShowErrorWindow(m_dataframe->GetErrorMessage(), &open);
          }
        }
        loadWindow = false;
      }

      ImGui::SameLine();
      if (ImGui::Button(" Cancel "))
      {
        loadWindow = false;
      }

      ImGui::End();
    }
  }
}

void UI::ShowRelation(void)
{
  if (!curr_filepath.empty())
  {
    ImGui::Text("Relation: ");
    ImGui::SameLine();
    ImGui::Text(curr_filepath.c_str());

    ImGui::Text("Attributes: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(m_dataframe->GetAttributeCount()).c_str());

    ImGui::Text("Instances: ");
    ImGui::SameLine();
    ImGui::Text(std::to_string(m_dataframe->GetInstanceCount()).c_str());
  }

}

void UI::ShowAttributes(void)
{
  if (!curr_filepath.empty())
  {
    std::vector<std::string> temp = m_dataframe->GetAttributeNameList();
    static int listbox_item_current = 1;
    if (ImGui::ListBox("Attributes", &listbox_item_current, StringItemsGetter, &temp, int(temp.size()), 12))
    {
      selected_att = temp[listbox_item_current];
      i_selected_att = listbox_item_current;
    }
  }
}

void UI::ShowSeleAttr(void)
{
  if (!curr_filepath.empty())
  {
    ImGui::Text("Name: ");
    ImGui::SameLine();
    ImGui::Text(selected_att.c_str());


    ImGui::Text("Statistic");

    struct Statistic
    {
      double Min;
      double Max;
      double Mean;
      double Median;
      std::vector<double> Mode;
    };
    static Statistic mStatistic;
    if (mUpdatable)
    {
      mStatistic.Min = FindMin(m_dataframe->GetInstances(), i_selected_att);
      mStatistic.Max = FindMax(m_dataframe->GetInstances(), i_selected_att);
      mStatistic.Mean = CalculateMean(m_dataframe->GetInstances(), i_selected_att);
      mStatistic.Median = CalculateMedian(m_dataframe->GetInstances(), i_selected_att);
      mStatistic.Mode = CalculateMode(m_dataframe->GetInstances(), i_selected_att);
    }
    //double min = FindMin(m_dataframe->GetInstances(), i_selected_att);
    ImGui::Text("Minimum : "); ImGui::SameLine(); ImGui::Text(std::to_string(mStatistic.Min).c_str());

    //double max = FindMax(m_dataframe->GetInstances(), i_selected_att);
    ImGui::Text("Maximum : "); ImGui::SameLine(); ImGui::Text(std::to_string(mStatistic.Max).c_str());

    //double mean = CalculateMean(m_dataframe->GetInstances(), i_selected_att);
    ImGui::Text("Mean : "); ImGui::SameLine(); ImGui::Text(std::to_string(mStatistic.Mean).c_str());

    //double median = CalculateMedian(m_dataframe->GetInstances(), i_selected_att);
    ImGui::Text("Median : "); ImGui::SameLine(); ImGui::Text(std::to_string(mStatistic.Median).c_str());

    //std::vector<double> mode = CalculateMode(m_dataframe->GetInstances(), i_selected_att);
    ImGui::Text("Mode :"); ImGui::SameLine();
    for (int i = 0; i < mStatistic.Mode.size(); ++i)
    {
      ImGui::Text(std::to_string(i + 1).c_str());
      ImGui::SameLine(); ImGui::Text(". "); ImGui::SameLine();
      ImGui::Text(std::to_string(mStatistic.Mode[i]).c_str());
    }
  }
}


void UI::PlotHistogram(void)
{
  if (!curr_filepath.empty())
  {
    Instance data = m_dataframe->GetInstance(size_t(i_selected_att));
    int size = data.GetAttributeCount();
    static float* values = new float(float(size));

    for (int i = 0; i < size; ++i)
    {
      values[i] = float(data.GetAttribute(i).AsDouble());
    }
    ImGui::PlotHistogram("Plot", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));
  }
}

void UI::Q1Q2(void)
{
  static bool Q1Q2Window = false;
  if (ImGui::Button("Q1 and Q2"))
    Q1Q2Window = !Q1Q2Window;

  if (Q1Q2Window)
  {
    ImGui::SetNextWindowSize(ImVec2(540, 500), ImGuiSetCond_Once);
    if (ImGui::Begin("Answer for Q1 and Q2", &Q1Q2Window, ImVec2(0, 0)))
    {
      LinearRegressionTest *test = new LinearRegressionTest(m_dataframe);

      std::vector<Equation> bestfits;
      std::vector<double> errors;
      size_t min, max;
      test->Answer(bestfits, errors, max, min);
      if (ImGui::CollapsingHeader("Question 1", (const char*)0, true, true))
      {
        ImGui::Text("Question 1 : Which attribute is the least related to the quality of wines?");
        ImGui::Text(" ");
        ImGui::Text("We think this will be an attribute that has most error in linear regression");
        ImGui::Text("Most error attributes: "); ImGui::SameLine();
        ImGui::Text(m_dataframe->GetAttributeName(max).c_str());
        ImGui::Text("Error: "); ImGui::SameLine();
        ImGui::Text(std::to_string(errors[max]).c_str());
      }
      ImGui::Separator();
      if (ImGui::CollapsingHeader("Question 2", (const char*)0, true, true))
      {
        ImGui::Text("Question 2 : Which attribute is the most related to the quality of wines?");
        ImGui::Text(" ");
        ImGui::Text("We think this will be an attribute that has least error in linear regression");
        ImGui::Text("Least error attributes: "); ImGui::SameLine();
        ImGui::Text(m_dataframe->GetAttributeName(min).c_str());
        ImGui::Text("Error: "); ImGui::SameLine();
        ImGui::Text(std::to_string(errors[min]).c_str());
      }
      ImGui::Separator();

      if (ImGui::CollapsingHeader("Best Slope line", (const char*)0, true, true))
      {
        ImGui::Text("Best slope line");
        std::vector<std::string> temp = m_dataframe->GetAttributeNameList();
        temp.pop_back();
        static int listbox_item_current = 1;
        ImGui::ListBox("Attributes", &listbox_item_current, StringItemsGetter, &temp, int(temp.size()), 12);

        struct Funcs
        {
          static float func(void*, float i) { return sinf(i * 0.1f); }
        };


        if (listbox_item_current != -1)
        {
          float data[2] = { float(bestfits[listbox_item_current].a) ,float(bestfits[listbox_item_current].b) };

          struct Funcs
          {
            static float func(void* data, int i) {
              float* fData = reinterpret_cast<float*>(data);
              return fData[0] * i + fData[1];
            }
          };


          //float(*func)(void*, int) = Funcs::func;
          //ImGui::PlotLines("Lines", func, data, 5, 0, NULL, 0.0f, 5.0f, ImVec2(0, 80));

          ImGui::Text(temp[listbox_item_current].c_str()); ImGui::SameLine();
          ImGui::Text(" and target class Quality. ");

          std::string line = "y = " + std::to_string(bestfits[listbox_item_current].a)
            + "x + " + std::to_string(bestfits[listbox_item_current].b);
          ImGui::Text(line.c_str());

          ImGui::Text("Error : "); ImGui::SameLine();
          ImGui::Text(std::to_string(errors[listbox_item_current]).c_str());
        }

      }
      ImGui::End();
    }
  }
}
void UI::Q3(void)
{
  static Q3Window window;
  if (ImGui::Button("Q3"))
    window.SetActive(!window.IsActive());

  window.Update();
}
void UI::RecommenderSystem(void)
{
  static bool RecWindow = false;
  if (ImGui::Button("Recommender System"))
    RecWindow = !RecWindow;

  if (RecWindow)
  {
    if (mRecommender->pRecommender == nullptr)
    {
      if (m_dataframe != nullptr)
        mRecommender->pRecommender = new WineRecommender(*m_dataframe);
      else return;
    }
    else if (mUpdatable)
    {
      delete mRecommender->pRecommender;
      mRecommender->pRecommender = new WineRecommender(*m_dataframe);
      mUpdatable = false;
    }

    ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_FirstUseEver);
    if (ImGui::Begin("Recommender System", &RecWindow, ImVec2(0, 0)))
    {
      ImGui::Text(" and target class Quality. ");

      mRecommender->pRecommender->AddIgnoreAttribute(std::string("quality"));
      size_t pick = curr_filepath.find("both");
      if (pick != std::string::npos)
      {
        mRecommender->pRecommender->AddIgnoreAttribute(std::string("wine type"));
      }
      else if (std::string::npos != curr_filepath.find("white"))
      {

      }
      else
      {

      }

      auto result = mRecommender->pRecommender->Recommend();

      ImGui::Text(std::to_string(result.front()).data());

      ImGui::End();
    }
  }
}

void UI::AddString(int index, std::string string)
{
  switch (index)
  {
  case 0:
    dk_testoutput.push_back(string);
    break;
  case 1:
    hb_testoutput.push_back(string);
    break;
  default:
    break;
  }
}

void UI::PrintString(int index, bool* p_open)
{
  ImGui::SetNextWindowPos(ImVec2(15, 50), ImGuiSetCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(900, 500), ImGuiSetCond_FirstUseEver);
  bool* open = p_open;
  size_t size = 0;
  switch (index)
  {
  case 0:
    size = dk_testoutput.size();
    if (ImGui::Begin("DK test output", open, ImGuiWindowFlags_MenuBar))
    {
      for (int i = 0; i < size; ++i)
      {
        ImGui::Text(dk_testoutput[i].c_str());
      }
      ImGui::End();
    }
    break;
  case 1:
    size = hb_testoutput.size();
    if (ImGui::Begin("HB test output", open, ImGuiWindowFlags_MenuBar))
    {
      for (int i = 0; i < size; ++i)
      {
        ImGui::Text(hb_testoutput[i].c_str());
      }
      ImGui::End();
    }
    break;
  }
}