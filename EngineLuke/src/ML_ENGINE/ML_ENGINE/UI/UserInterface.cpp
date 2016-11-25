#include "UserInterface.h"
#include "../FileIO/FileSystem.h"

UI* UI::s_pinstance = NULL;

UI::UI(void) 
{
}
UI::~UI(void)
{
}

void UI::Initialize(void)
{
}
void UI::Update(void)
{
    MainWindow();
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
            ImGui::Text("Relation: "); 
            ImGui::SameLine(); 
            ImGui::Text(" "); 

            ImGui::Text("Attributes: ");
            ImGui::SameLine();
            ImGui::Text(" ");

            ImGui::Text("Instances: ");
            ImGui::SameLine();
            ImGui::Text(" ");
          
        }

        if (ImGui::CollapsingHeader("Attributes", (const char*)0, true, true))
        {
            /*
            for()//each attribute
            {
            ImGui::Text();     //Number
            ImGui::SameLine();
            ImGui::Text(" ");  //Attribute name
            }
            */

            std::vector<std::string> temp;
            static int listbox_item_current = 1;
            if (ImGui::ListBox("Attributes", &listbox_item_current, StringItemsGetter, &temp, int(temp.size()), 10))
                selected_att = temp[listbox_item_current];
        }

        ImGui::NextColumn();

        if (ImGui::CollapsingHeader("Selected Attributes", (const char*)0, true, true))
        {
            ImGui::Text("Name: ");
            ImGui::SameLine();
            ImGui::Text(selected_att.c_str());

           
            ImGui::Separator();
            if(ImGui::TreeNode("Statistic"))
            {
                ImGui::Text("Minimum");
                ImGui::Text("Maximum");
                ImGui::Text("Mean");
                ImGui::Text("StdDev");
                ImGui::TreePop();
            }
            
         
        }
        if (ImGui::CollapsingHeader("Plot", (const char*)0, true, true))
        {
            std::vector<std::string> temp;
            //ImGui::PlotHistogram("Plot", ValueGetter, &temp, temp.size(), 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));
        }
        ImGui::End();
    }
}

void UI::LoadCSVfile(void)
{
    static bool spaceLoadWindow = false;
    if (ImGui::Button("Choose"))
        spaceLoadWindow = true;

    ImGui::SameLine();
    ImGui::Text(curr_filepath.c_str());

    ImGui::Separator();

    if (!spaceLoadWindow)
        return;

    if (spaceLoadWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_Always);
        if (ImGui::Begin(" File Load ", &spaceLoadWindow, ImVec2(0, 0)))
        {
            std::vector<std::string> fileList = GetFilesNameFromDirectory(std::string("Data"));
            static int currIndex = -1;

            ImGui::Combo(" Data File ", &currIndex, StringItemsGetter, &fileList, int(fileList.size()));

            if (ImGui::Button(" Load "))
            {
                if (currIndex != -1)
                    curr_filepath = fileList[currIndex];
                spaceLoadWindow = false;
            }

            ImGui::SameLine();
            if (ImGui::Button(" Cancel "))
            {
                spaceLoadWindow = false;
            }

            ImGui::End();
        }
    }
}

void UI::LoadMethod(void)
{

}

void UI::TempTestWindow(void)
{

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
