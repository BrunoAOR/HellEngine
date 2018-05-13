#ifndef __H_MODULE_IMGUI__
#define __H_MODULE_IMGUI__

#include <string>
#include "ImGuizmo/ImGuizmo.h"
#include "Module.h"

class ModuleImGui :
	public Module
{
public:

	ModuleImGui();
	
	/* Destructor */
	~ModuleImGui();

	/* Called before render is available */
	bool Init();

	/* Called each loop iteration */
	UpdateStatus PreUpdate();

	/* Called each loop iteration */
	UpdateStatus Update();

	/* Called before quitting */
	bool CleanUp();

	void Render() const;

public:

	static ImGuizmo::OPERATION mCurrentGizmoOperation;

private:

	/* Opens the browser to the provided link */
	void LinkToWebPage(const char* text, const char* url);

	/* Draws the About window */
	void ShowAboutWindow(float mainMenuBarHeight, bool* pOpen);

	/* Draws the Editor Camera window */
	void ShowEditorCameraWindow(float mainMenuBarHeight, bool* pOpen);

	/* Draws the OpenGL options window */
	void ShowOpenGLWindow(float mainMenuBarHeight, bool* pOpen);

    /*Draws the Text Editor window*/
    void ShowTextEditorWindow(float mainMenuBarHeight, bool* pOpen);

	/* Draws the QuadTree options window*/
	void ShowQuadTreeWindow(float mainMenuBarHeight, bool* pOpen);

	/* Draws the Raycast testing window */
	void ShowRaycastTestWindow(float mainmenuBarHeight, bool* pOpen);

    /* Draws Guizmo */
    void DrawGuizmo();
	
private:

	std::string licenseString;

};

#endif // !__H_MODULE_IMGUI__
