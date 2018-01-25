#ifndef __H_MODULE_IMGUI__
#define __H_MODULE_IMGUI__

#include <string>
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

private:

	/* Opens the browser to the provided link */
	void LinkToWebPage(const char* text, const char* url);

private:

	std::string licenseString;

    /*Editor variables*/
    float fovh;
    float fovv;
    float movSpeed;
    float rotSpeed;
    float zoomSpeed;
    float nearPlane;
    float farPlane;
    float aspectRatio;
    bool active;
    bool frustumCulling;
    bool isActiveCamera;
};

#endif // !__H_MODULE_IMGUI__
