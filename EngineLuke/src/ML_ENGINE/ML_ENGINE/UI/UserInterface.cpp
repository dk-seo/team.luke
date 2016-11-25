#define _CRT_SECURE_NO_WARNINGS

#include "UserInterface.h"
#include "FileIO\FileSystem.h"
#include "UI\Imgui\imconfig.h"
//For Q3
#include "ML\Regression\Equation.h"
#include "ML\Regression\LinearRegression.h"
#include "Test\LinearRegressionTest.h"

UI* UI::s_pinstance = NULL;

UI::UI(void) 
{
}
UI::~UI(void)
{
}

void UI::Initialize(void)
{
    m_dataframe = new Dataframe();
}
void UI::Update(void)
{
    MainWindow();

    UpdateMethodWindow();
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
        if (ImGui::CollapsingHeader("Plot", (const char*)0, true, true))
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
    ImGui::SetNextWindowPos(ImVec2(930, 50), ImGuiSetCond_Once);
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
    ImGui::SetNextWindowPos(ImVec2(930, 130), ImGuiSetCond_Once);
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
                    curr_filepath = fileList[currIndex];

                    std::string path = "Data/" + curr_filepath;                   

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
		if (ImGui::ListBox("Attributes", &listbox_item_current, StringItemsGetter, &temp, temp.size(), 12))
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


        //ImGui::Separator();
        if (ImGui::TreeNode("Statistic"))
        {
            //we did this statistic prob in PA1, where is the code?
            //ask hb 
            ImGui::Text("Minimum");
            ImGui::Text("Maximum");
            ImGui::Text("Mean");
            ImGui::Text("StdDev");
            ImGui::TreePop();
        }

    }
}


void UI::PlotHistogram(void)
{
	if (!curr_filepath.empty())
	{
		Instance data = m_dataframe->GetInstance(size_t(i_selected_att));
		int size = data.GetAttributeCount();
		static float* values = new float(size);

		for (int i = 0; i < size; ++i)
		{
			values[i] = data.GetAttribute(i).AsDouble();
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
				ImGui::ListBox("Attributes", &listbox_item_current, StringItemsGetter, &temp, temp.size(), 12);

				if (listbox_item_current != -1)
				{
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
    static bool Q3Window = false;
    if (ImGui::Button("Q3"))
        Q3Window = !Q3Window;

    if (Q3Window)
    {
        ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Answer for Q3", &Q3Window, ImVec2(0, 0)))
        {

            ImGui::End();
        }
    }
}
void UI::RecommenderSystem(void)
{
    static bool RecWindow = false;
    if (ImGui::Button("Recommender System"))
        RecWindow = !RecWindow;

    if (RecWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("Recommender System", &RecWindow, ImVec2(0, 0)))
        {

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
