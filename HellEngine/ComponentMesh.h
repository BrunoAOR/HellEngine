#ifndef __H_COMPONENT_MESH__
#define __H_COMPONENT_MESH__

#include "Component.h"

class ComponentMesh :
	public Component
{
public:
	ComponentMesh();
	virtual ~ComponentMesh() override;

	virtual void OnEditor() override;
};

#endif // !__H_COMPONENT_MESH__
