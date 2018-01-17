#ifndef __H_MODULE_TRIANGLE__
#define __H_MODULE_TRIANGLE__

#include "Module.h"

class ModuleTriangle :
	public Module
{
public:
	ModuleTriangle();

	/* Destructor */
	~ModuleTriangle();

	/* Called after all baisc Engine Inits */
	bool Init();

	/* Called each loop iteration */
	UpdateStatus Update();

	float v[3][3];
	float c[3][3];

private:
	void LogInfo() const;

	int activeVertex;
	int activeColor;
	float moveSpeed = 25.0f;
	const float colorChangeSpeed = 0.0625f;
};

#endif // !__H_MODULE_TRIANGLE__
