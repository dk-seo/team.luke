#include "Q3Window.h"
#include "UI\Imgui\imgui.h"
#include "UI\Imgui\imgui_impl_dx11.h"

Q3Window::Q3Window()
	:_active(false)
{
}

Q3Window::~Q3Window()
{
}

void Q3Window::Update()
{
	ImGui::SetNextWindowSize(ImVec2(350, 100), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Answer for Q3", &_active, ImVec2(0, 0)))
	{
		ImGui::End();
	}
}