#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> /* Has to be included AFTER Windows.h */
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "Application.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

ModuleImGui::ModuleImGui()
{
	licenseString = std::string("")
		+ "MIT License\n"
		+ "\n"
		+ "Copyright(c) 2017 Marc Campins, Bruno Ortiz, Sergi Ramirez, Daniel Reyes, Mario Rodriguez\n"
		+ "\n"
		+ "Permission is hereby granted, free of charge, to any person obtaining a copy "
		+ "of this software and associated documentation files(the \"Software\"), to deal "
		+ "in the Software without restriction, including without limitation the rights "
		+ "to use, copy, modify, merge, publish, distribute, sublicense, and/or sell "
		+ "copies of the Software, and to permit persons to whom the Software is "
		+ "furnished to do so, subject to the following conditions:\n"
		+ "\n"
		+ "The above copyright notice and this permission notice shall be included in all "
		+ "copies or substantial portions of the Software.\n"
		+ "\n"
		+ "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
		+ "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
		+ "FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE "
		+ "AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
		+ "LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, "
		+ "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
		+ "SOFTWARE.";
}

/* Destructor */
ModuleImGui::~ModuleImGui()
{
}

/* Called before render is available */
bool ModuleImGui::Init()
{
	// Setup ImGui binding
	ImGui_ImplSdlGL3_Init(App->window->window);

	// Setup style
	//ImGui::StyleColorsClassic();
	ImGui::StyleColorsDark();

	return true;
}

/* Called each loop iteration */
UpdateStatus ModuleImGui::PreUpdate()
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called each loop iteration */
UpdateStatus ModuleImGui::Update()
{
	static bool shouldQuit = false;
	static bool showAbout = false;
	static bool showDemoWindow = false;
	static bool rendererWireFrame = false;
	static bool rendererRotate = false;

	float mainMenuBarHeight;

	if (ImGui::BeginMainMenuBar())
	{
		mainMenuBarHeight = ImGui::GetWindowHeight();
		if (ImGui::BeginMenu("Main"))
		{
			ImGui::MenuItem("Show demo window", nullptr, &showDemoWindow);
			ImGui::Separator();
			
			ImGui::MenuItem("Quit", nullptr, &shouldQuit);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Configuration"))
		{
			if (ImGui::BeginMenu("Rendering"))
			{
				if (ImGui::MenuItem("Wireframe", nullptr, &rendererWireFrame))
				{
					App->renderer->wireframe = !App->renderer->wireframe;
				}
				if (ImGui::MenuItem("Rotate", nullptr, &rendererRotate))
				{
					App->renderer->shouldRotate = !App->renderer->shouldRotate;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About HellEngine", nullptr, &showAbout);

			ImGui::MenuItem("Yes, HELP US...", nullptr, false, false);
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text("I am a helpful tooltip!");
				ImGui::EndTooltip();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	if (showAbout)
	{
		ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
		ImGui::SetNextWindowSize(ImVec2(600, 600));
		ImGui::Begin("About", &showAbout, ImGuiWindowFlags_NoCollapse);

		LinkToWebPage("Hell Engine v0.0.1", "https://github.com/BrunoAOR/HellEngine");
		ImGui::TextWrapped("The one and only 3D Game Engine that guarantees an increased developer suicide rate!");
		ImGui::Text("");
		
		ImGui::Text("Authors:");
		ImGui::Bullet(); LinkToWebPage("Marc Campins", "https://github.com/McCampins");
		ImGui::Bullet(); LinkToWebPage("Bruno Ortiz", "https://github.com/BrunoAOR");
		ImGui::Bullet(); LinkToWebPage("Sergi Ramirez", "https://github.com/Orphen5");
		ImGui::Bullet(); LinkToWebPage("Daniel Reyes", "https://github.com/Winour");
		ImGui::Bullet(); LinkToWebPage("Mario Rodriguez", "https://github.com/Mac092");
		ImGui::Text("");
		
		ImGui::Text("3rd party libraries used:");
		ImGui::Bullet(); LinkToWebPage("SDL 2.0.4", "https://www.libsdl.org");
		ImGui::Bullet(); LinkToWebPage("SDL Image 2.0.0", "https://www.libsdl.org/projects/SDL_image/");
		ImGui::Bullet(); LinkToWebPage("SDL Mixer 2.0.0", "https://www.libsdl.org/projects/SDL_mixer/");
		ImGui::Bullet(); LinkToWebPage("Brofiler 1.1.2", "https://github.com/bombomby/brofiler");
		ImGui::Bullet(); LinkToWebPage("MathGeoLib 1.5", "https://github.com/juj/MathGeoLib");
		ImGui::Bullet(); LinkToWebPage("JSON for Modern C++ 3.0.1", "https://github.com/nlohmann/json");
		ImGui::Bullet(); LinkToWebPage("ImGui 1.53", "https://github.com/ocornut/imgui");
		ImGui::Bullet(); LinkToWebPage("Glew 2.1.0", "http://glew.sourceforge.net");
		ImGui::Bullet(); LinkToWebPage("OpenGL 3.1", "https://www.khronos.org/opengl/");
		ImGui::Text("");

		LinkToWebPage("License:", "https://github.com/BrunoAOR/HellEngine/blob/master/LICENSE");
		ImGui::Text("");
		ImGui::TextWrapped(licenseString.c_str());
		
		ImGui::End();
	}

	ImGui::Render();

	if (shouldQuit)
		return UpdateStatus::UPDATE_STOP;
	else
		return UpdateStatus::UPDATE_CONTINUE;
}

/* Called before quitting */
bool ModuleImGui::CleanUp()
{
	ImGui_ImplSdlGL3_Shutdown();
	return true;
}

void ModuleImGui::LinkToWebPage(const char* text, const char* url)
{
	if (ImGui::Selectable(text, false))
		ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
}
