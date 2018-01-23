#ifndef __H_MODULE_EDITOR_CAMERA__
#define __H_MODULE_EDITOR_CAMERA__

#include "MathGeoLib/src/Geometry/Frustum.h"
#include "Module.h"

class ModuleEditorCamera :
	public Module
{
public:

	ModuleEditorCamera();
	~ModuleEditorCamera();

	bool Init();
	UpdateStatus Update();

	void SetPosition(float x, float y, float z);
	float* GetViewMatrix();
	float* GetProjectionMatrix();
	
private:

	Frustum frustrum;
	const float moveSpeed = 1;
};

#endif // !__H_MODULE_EDITOR_CAMERA__
