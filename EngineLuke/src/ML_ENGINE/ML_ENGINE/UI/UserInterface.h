/******************************************************************************/
/*!
\file UserInterface.h
\project CS399_TeamLuke
\author Jiyun Kang

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include <string>
#include <vector>

#include "UI\Imgui\imgui.h"
#include "UI\Imgui\imgui_impl_dx11.h"
#include "ML\Dataframe\Dataframe.h"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

class UI
{
public:

  static inline void Create(void)
  {
    if (!s_pinstance)
    {
      s_pinstance = new UI();
    }
  }
  static inline void Delete(void)
  {
    if (s_pinstance)
    {
      delete s_pinstance;
      s_pinstance = NULL;
    }
  }
  static inline UI& Get(void) { return *s_pinstance; }

  void Initialize(void);
  void Shutdown(void);
  void Update(void);

  void MainWindow(void);

  void ShowErrorWindow(std::string message, bool* open);

  //for test 
  void AddString(int index, std::string string);
  void PrintString(int index, bool* p_open);

  inline Dataframe* GetDataframe() { return m_dataframe; }

private:


  UI(void);
  ~UI(void);
  static UI* s_pinstance;

  void UpdateMethodWindow(void);
  void UpdateQuestionsWindow(void);

  void LoadCSVfile(void);
  void ShowRelation(void);
  void ShowAttributes(void);
  void ShowSeleAttr(void);
  void PlotHistogram(void);

  ///////////////////////
  void Q1Q2(void);
  void Q3(void);
  void RecommenderSystem(void);
  ///////////////////////

  std::string curr_filepath;
  std::string selected_att;
  int         i_selected_att;

  Dataframe *m_dataframe;

  //for test
  std::vector<std::string> dk_testoutput;
  std::vector<std::string> hb_testoutput;


};