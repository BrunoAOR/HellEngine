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

private:

	Frustum frustrum;
};

#endif // !__H_MODULE_EDITOR_CAMERA__
