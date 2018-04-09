#include "ImGui_GameDLL/imgui.h"
#include "dllMain.h"

void SetDLLImguiContext(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
}