#pragma once
#include "../UI/Imgui/imgui.h"
#include "../UI/Imgui/imgui_impl_dx11.h"
#include <string>
#include <vector>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

class UI
{
public:

    static void Create(void)
    {
        if (!s_pinstance)
        {
            s_pinstance = new UI();
        }
    }
    static void Delete(void)
    {
        if (s_pinstance)
        {
            delete s_pinstance;
            s_pinstance = NULL;
        }
    }
    static UI& Get(void) { return *s_pinstance; }
  
    void Initialize(void);
    void Shutdown(void);
    void Update(void);

    void MainWindow(void);

    //for test 
    void TempTestWindow(void);
    void AddString(int index, std::string string);
    void PrintString(int index, bool* p_open);

private:

    
    UI(void);
    ~UI(void);
    static UI* s_pinstance;

    void LoadCSVfile(void);
    void LoadMethod(void);

    std::string curr_filepath;
    std::string curr_method;
    std::string selected_att;
    
    std::vector<std::string> methodList;


    //for test
    std::vector<std::string> dk_testoutput;
    std::vector<std::string> hb_testoutput;

   
};