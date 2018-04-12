#ifdef _DEBUG
	#pragma comment(lib, "../Debug/HellEngine.lib")
#else
	#pragma comment(lib, "../Release/HellEngine.lib")
#endif // _DEBUG

#include "ImGui_GameDLL/imgui.h"
#include "dllMain.h"

void SetDLLImguiContext(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
}