#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h> /* Has to be included AFTER Windows.h */
#include "Glew/include/glew.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl_gl3.h"
#include "Application.h"
#include "ModuleEditorCamera.h"
#include "ModuleImGui.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "globals.h"

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
	static bool showDemoWindow = false;
	static bool showAbout = false;
	static bool showCameraWindow = false;
	static bool showOpenGLWindow = false;
    static bool showTextEditorWindow = false;
	static bool showHierarchyWindow = true;
	static bool showInspectorWindow = true;
	static bool showQuadTreeWindow = false;

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
			ImGui::MenuItem("Debug Mode", nullptr, &DEBUG_MODE);

			ImGui::Separator();

			ImGui::MenuItem("Camera", nullptr, &showCameraWindow);
			ImGui::MenuItem("OpenGL", nullptr, &showOpenGLWindow);

			ImGui::Separator();

			ImGui::MenuItem("QuadTree", nullptr, &showQuadTreeWindow);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Text Editor", nullptr, &showTextEditorWindow);
			ImGui::MenuItem("Hierarchy", nullptr, &showHierarchyWindow);
			ImGui::MenuItem("Inspector", nullptr, &showInspectorWindow);
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
		ShowAboutWindow(mainMenuBarHeight, &showAbout);
	}

	if (showCameraWindow)
	{
		ShowEditorCameraWindow(mainMenuBarHeight, &showCameraWindow);
	}

	if (showOpenGLWindow)
	{
		ShowOpenGLWindow(mainMenuBarHeight, &showOpenGLWindow);
	}

    if (showTextEditorWindow) 
    {
        ShowTextEditorWindow(mainMenuBarHeight, &showTextEditorWindow);
    }

	if (showHierarchyWindow)
	{
		App->scene->OnEditorHierarchy(mainMenuBarHeight, &showHierarchyWindow);
	}

	if (showInspectorWindow)
	{
		App->scene->OnEditorInspector(mainMenuBarHeight, &showInspectorWindow);
	}

	if (showQuadTreeWindow)
	{
		ShowQuadTreeWindow(mainMenuBarHeight, &showQuadTreeWindow);
	}

	ImGui::Render();

	if (shouldQuit)
	{
		return UpdateStatus::UPDATE_STOP;
	}
	else
	{
		return UpdateStatus::UPDATE_CONTINUE;
	}
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

void ModuleImGui::ShowAboutWindow(float mainMenuBarHeight, bool* pOpen)
{
	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(600, 600));
	ImGui::Begin("About", pOpen, ImGuiWindowFlags_NoCollapse);

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

void ModuleImGui::ShowEditorCameraWindow(float mainMenuBarHeight, bool* pOpen)
{
	static float fovh = 0.0f;
	static float fovv = 0.0f;
	static float nearPlane = App->editorCamera->camera->GetNearPlaneDistance();
	static float farPlane = App->editorCamera->camera->GetFarPlaneDistance();
	static float aspectRatio = 0.0f;
	static bool active = false;
	static bool frustumCulling = false;
	static bool isActiveCamera = true;

	ImGui::SetNextWindowPos(ImVec2((float)App->window->getWidth() - 300, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(300, 600));
	ImGui::Begin("Camera", pOpen, ImGuiWindowFlags_NoCollapse);

	ImGui::Checkbox("Active", &active);

	static float front[3] = { 0, 0, 0 };
	static const float* editorFront = nullptr;
	editorFront = App->editorCamera->camera->GetFront();
	front[0] = editorFront[0];
	front[1] = editorFront[1];
	front[2] = editorFront[2];
	ImGui::InputFloat3("Front", front, 2);

	static float up[3] = { 0, 0, 0 };
	static const float* editorUp = nullptr;
	editorUp = App->editorCamera->camera->GetUp();
	up[0] = editorUp[0];
	up[1] = editorUp[1];
	up[2] = editorUp[2];
	ImGui::InputFloat3("Up", up, 3);

	static float position[3] = { 0, 0, 0 };
	static const float* editorPos = nullptr;
	editorPos = App->editorCamera->camera->GetPosition();
	position[0] = editorPos[0];
	position[1] = editorPos[1];
	position[2] = editorPos[2];
	if (ImGui::InputFloat3("Position", position))
		App->editorCamera->camera->SetPosition(position[0], position[1], position[2]);

	ImGui::SliderFloat("Mov Speed", &App->editorCamera->moveSpeed, 1.0f, 25.0f);
	ImGui::SliderFloat("Rot Speed", &App->editorCamera->rotationSpeed, 1.0f, 25.0f);
	ImGui::SliderFloat("Zoom Speed", &App->editorCamera->zoomSpeed, 10.0f, 200.0f);

	ImGui::Checkbox("Frustum Culling", &frustumCulling);
	if (ImGui::SliderFloat("Near Plane", &nearPlane, 0.01f, 30.0f))
		App->editorCamera->camera->SetNearPlaneDistance(nearPlane);
	if (ImGui::SliderFloat("Far Plane", &farPlane, 50.0f, 2000.0f))
		App->editorCamera->camera->SetFarPlaneDistance(farPlane);

	fovh = App->editorCamera->camera->GetHorizontalFOV();
	ImGui::SliderFloat("Fov H", &fovh, 0.1f, 180.0f);
	fovv = App->editorCamera->camera->GetVerticalFOV();
	if (ImGui::SliderFloat("Fov V", &fovv, 0.1f, 180.0f))
		App->editorCamera->camera->SetFOV(fovv);
	aspectRatio = App->editorCamera->camera->GetAspectRatio();
	ImGui::SliderFloat("Aspect Ratio", &aspectRatio, 0.1f, 5.0f);

	static float backgroundColor[3] = { 0.0f, 0.0f, 0.0f };
	if (ImGui::ColorEdit3("color 1", backgroundColor))
	{
		App->editorCamera->camera->SetBackground(backgroundColor[0], backgroundColor[1], backgroundColor[2]);
	}

	ImGui::Checkbox("Is Active Camera", &isActiveCamera);

	ImGui::End();
}

void ModuleImGui::ShowOpenGLWindow(float mainMenuBarHeight, bool * pOpen)
{
	static bool alphaTest = true;
	static bool depthTest = true;
	static bool cullFace = true;
	static bool lighting = false;
	static bool colorMaterial = true;
	static bool texture2D = true;
	static bool fog = false;
	static int fogMode = GL_EXP;
	static int previousFogMode = GL_EXP;
	static float fogDensity = 1.0f;
	static float fogDistance[2] = { 0.0f, 1.0f };
	static float fogColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static float ambientLightColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	static bool extraLight = false;
	static float extraLightColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
	static int polygonMode = GL_FILL;
	static int previousPolygonMode = GL_FILL;
	static int referenceGridTrackingBehaviour = 0;
	static int previousReferenceGridTrackingBehaviour = 0;


	ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
	ImGui::SetNextWindowSize(ImVec2(450, 600));
	ImGui::Begin("OpenGL options", pOpen, ImGuiWindowFlags_NoCollapse);

	if (ImGui::Checkbox("Alpha Test", &alphaTest))
	{
		App->renderer->ToggleOpenGLCapability(alphaTest, GL_BLEND);
	}
	if (ImGui::Checkbox("Depth Test", &depthTest))
	{
		App->renderer->ToggleOpenGLCapability(depthTest, GL_DEPTH_TEST);
	}
	if (ImGui::Checkbox("Backface culling", &cullFace))
	{
		App->renderer->ToggleOpenGLCapability(cullFace, GL_CULL_FACE);
	}
	if (ImGui::Checkbox("Lighting", &lighting))
	{
		App->renderer->ToggleOpenGLCapability(lighting, GL_LIGHTING);
	}
	if (ImGui::Checkbox("Color", &colorMaterial))
	{
		App->renderer->ToggleOpenGLCapability(colorMaterial, GL_COLOR_MATERIAL);
	}
	if (ImGui::Checkbox("Textures", &texture2D))
	{
		App->renderer->ToggleOpenGLCapability(texture2D, GL_TEXTURE_2D);
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Fog", &fog))
	{
		App->renderer->ToggleOpenGLCapability(fog, GL_FOG);
	}
	ImGui::Text("Fog mode:"); ImGui::SameLine();
	ImGui::RadioButton("Linear", &fogMode, GL_LINEAR); ImGui::SameLine();
	ImGui::RadioButton("Exp", &fogMode, GL_EXP); ImGui::SameLine();
	ImGui::RadioButton("Exp sqrd", &fogMode, GL_EXP2);

	if (previousFogMode != fogMode)
	{
		previousFogMode = fogMode;
		App->renderer->SetFogMode(fogMode);
	}
	switch (fogMode)
	{
	case GL_LINEAR:
		if (ImGui::DragFloat2("Fog distance", fogDistance, 0.05f, 0.0f, 100.0f, "%.2f"))
		{
			App->renderer->setFogStartAndEnd(fogDistance[0], fogDistance[1]);
		}
		break;
	case GL_EXP:
	case GL_EXP2:
		if (ImGui::DragFloat("Fog density", &fogDensity, 0.01f, 0.0f, 1.0f, "%.2f"))
		{
			App->renderer->SetFogDensity(fogDensity);
		}
		break;
	}
	if (ImGui::ColorEdit3("Fog color", fogColor))
	{
		App->renderer->SetFogColor(fogColor);
	}

	ImGui::Separator();

	ImGui::Text("Lights:");
	if (ImGui::ColorEdit3("Ambient Light color", ambientLightColor))
	{
		App->renderer->SetAmbientLightColor(ambientLightColor);
	}
	if (ImGui::Checkbox("Extra ambient light", &extraLight))
	{
		App->renderer->ToggleOpenGLCapability(extraLight, GL_LIGHT0);
	}
	if (extraLight)
	{
		if (ImGui::ColorEdit3("Extra Light color", extraLightColor))
		{
			App->renderer->SetLightColor(GL_LIGHT0, extraLightColor);
		}
	}

	ImGui::Separator();

	ImGui::Text("Polygon drawing mode:");
	ImGui::RadioButton("Fill", &polygonMode, GL_FILL); ImGui::SameLine();
	ImGui::RadioButton("Lines", &polygonMode, GL_LINE); ImGui::SameLine();
	ImGui::RadioButton("Point", &polygonMode, GL_POINT);
	if (polygonMode != previousPolygonMode)
	{
		previousPolygonMode = polygonMode;
		App->renderer->SetPolygonDrawMode(polygonMode);
	}

	ImGui::Separator();

	ImGui::Text("Reference Grid:");
	ImGui::Checkbox("Active", &App->renderer->groundGridInfo.active);
	ImGui::Text("Tracking:");
	ImGui::RadioButton("Inactive", &referenceGridTrackingBehaviour, 0); ImGui::SameLine();
	ImGui::RadioButton("Continuous", &referenceGridTrackingBehaviour, 1); ImGui::SameLine();
	ImGui::RadioButton("Discrete", &referenceGridTrackingBehaviour, 2);
	if (previousReferenceGridTrackingBehaviour != referenceGridTrackingBehaviour)
	{
		previousReferenceGridTrackingBehaviour = referenceGridTrackingBehaviour;
		switch (referenceGridTrackingBehaviour)
		{
		case 0:
			App->renderer->groundGridInfo.tracking = false;
			break;
		case 1:
			App->renderer->groundGridInfo.tracking = true;
			App->renderer->groundGridInfo.continuousTracking = true;
			break;
		case 2:
			App->renderer->groundGridInfo.tracking = true;
			App->renderer->groundGridInfo.continuousTracking = false;
			break;
		default:
			break;
		}
	}

	ImGui::End();
}

void ModuleImGui::ShowTextEditorWindow(float mainMenuBarHeight, bool* pOpen) 
{
	static char text[1024 * 16] = "";
	static char fileName[256] = "";
	static std::string lastLog = "";
	static bool showExtendedSaveMenu = false;
	static bool showExtendedLoadMenu = false;

    ImGui::SetNextWindowPos(ImVec2(0, mainMenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(450, 570));
    ImGui::Begin("Visual Studio 2025", pOpen, ImGuiWindowFlags_NoCollapse);
    ImGui::InputTextMultiline("", text, IM_ARRAYSIZE(text), ImVec2(430.f, ImGui::GetTextLineHeight() * 32), ImGuiInputTextFlags_AllowTabInput); 
    
    ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));
	if (ImGui::Button("Save", ImVec2(125.0f, 25.0f)))
	{
		showExtendedSaveMenu = true;
		showExtendedLoadMenu = false;
	}

    ImGui::SameLine(0.0f, 42.0f);

	if (ImGui::Button("Load", ImVec2(125.0f, 25.0f)))
	{
		showExtendedLoadMenu = true;
		showExtendedSaveMenu = false;
	}

	if (showExtendedSaveMenu)
	{
		ImGui::Text("Are you sure you want to save?");
		if (ImGui::Button("Confirm Save", ImVec2(125.0f, 25.0f)))
		{
			showExtendedSaveMenu = false;
			if (SaveTextFile(fileName, text))
				lastLog = "File saved.";
			else
				lastLog = "File could NOT be saved.";
		}

		ImGui::SameLine(0.0f, 42.0f);

		if (ImGui::Button("Cancel Save", ImVec2(125.0f, 25.0f)))
			showExtendedSaveMenu = false;
	}

	if (showExtendedLoadMenu)
	{
		ImGui::Text("Are you sure you want to load?");
		if (ImGui::Button("Confirm Load", ImVec2(125.0f, 25.0f)))
		{
			showExtendedLoadMenu = false;
			std::string fileContent;
			if (LoadTextFile(fileName, fileContent))
			{
				lastLog = "File loaded.";
				strcpy_s(text, 1024 * 16, fileContent.c_str());
			}
			else
				lastLog = "File could NOT be loaded.";
		}

		ImGui::SameLine(0.0f, 42.0f);

		if (ImGui::Button("Cancel Load", ImVec2(125.0f, 25.0f)))
			showExtendedLoadMenu = false;
	}

	ImGui::Text("Last log: ");
	ImGui::SameLine();
	ImGui::Text(lastLog.c_str());

    ImGui::End();
}

void ModuleImGui::ShowQuadTreeWindow(float mainMenuBarHeight, bool * pOpen)
{
	static int quadTreeOption = 0;
	static int previousQuadTreeOption = -1;
	static bool minMaxChanged = false;
	
	ImGui::RadioButton("None", &quadTreeOption, 0);

	ImGui::Separator();

	static float minPoint[3] = { 0,0,0 };
	static float maxPoint[3] = { 0,0,0 };
	ImGui::RadioButton("Fixed", &quadTreeOption, 1);
	if (ImGui::InputFloat3("Min point", minPoint, 2))
		minMaxChanged = true;
	if (ImGui::InputFloat3("Max point", maxPoint, 2))
		minMaxChanged = true;

	ImGui::Separator();

	ImGui::RadioButton("Adaptive", &quadTreeOption, 2);

	{
		ImGui::Separator();

		ImGui::Text("QuadTree usage test:");
		ImGui::Text("Primitive: AABB");
		static float aabbMinPoint[3] = { -1,-1,-1 };
		static float aabbMaxPoint[3] = { 1,1,1 };
		ImGui::InputFloat3("AABB Min point", aabbMinPoint, 2);
		ImGui::InputFloat3("AABB Max point", aabbMaxPoint, 2);

		ImGui::Text("Randomly positions spheres:");
		static int spheresCount = 25;
		static float spawnMinPoint[3] = { -25,-25,-25 };
		static float spawnMaxPoint[3] = { 25,25,25 };
		ImGui::InputInt("Spheres count", &spheresCount);
		ImGui::InputFloat3("Spawn Min point", spawnMinPoint, 1);
		ImGui::InputFloat3("Spawn Max point", spawnMaxPoint, 1);

		if (ImGui::Button("Test Collisions"))
		{
			if (aabbMinPoint[0] < aabbMaxPoint[0] && aabbMinPoint[1] < aabbMaxPoint[1] && aabbMinPoint[2] < aabbMaxPoint[2]
				&& spawnMinPoint[0] < spawnMaxPoint[0] && spawnMinPoint[1] < spawnMaxPoint[1] && spawnMinPoint[2] < spawnMaxPoint[2]
				&& spheresCount > 0)
			{
				float3 aabbMin(minPoint[0], minPoint[1], minPoint[2]);
				float3 aabbMax(maxPoint[0], maxPoint[1], maxPoint[2]);
				float3 spawnMin(spawnMinPoint[0], spawnMinPoint[1], spawnMinPoint[2]);
				float3 spawnMax(spawnMaxPoint[0], spawnMaxPoint[1], spawnMaxPoint[2]);
				App->scene->TestCollisionChecks(aabbMin, aabbMax, spawnMin, spawnMax, spheresCount);
			}
			else
				LOGGER("Invalid test conditions!");
		}
	}

	if (quadTreeOption != previousQuadTreeOption || minMaxChanged && quadTreeOption == 1)
	{
		previousQuadTreeOption = quadTreeOption;
		minMaxChanged = false;
		switch (quadTreeOption)
		{
		case 0:
			App->scene->UnloadSceneFixedQuadTree();
			break;
		case 1:
			App->scene->UnloadSceneFixedQuadTree();
			if (minPoint[0] < maxPoint[0] && minPoint[1] < maxPoint[1] && minPoint[2] < maxPoint[2])
			{
				App->scene->GenerateSceneFixedQuadTree(minPoint, maxPoint);
			}
			else
			{
				previousQuadTreeOption = 0;
				quadTreeOption = 0;
			}
			break;
		case 2:
			App->scene->GenerateSceneAdaptiveQuadTree();
			break;
		default:
			break;
		}
	}
}
