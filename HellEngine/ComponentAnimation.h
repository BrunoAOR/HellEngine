#ifndef __H_COMPONENT_ANIMATION__
#define __H_COMPONENT_ANIMATION__

#include <map>
#include <string>
#include <vector>
#include "Component.h"
#include "TextureConfiguration.h"
#include "TextureInfo.h"
#include "globals.h"
#include "ModuleAnimation.h"
#include "ModuleTextureManager.h"

class ComponentAnimation :
	public Component
{
public:

	ComponentAnimation(GameObject* owner);
	virtual ~ComponentAnimation() override;

	virtual void Update() override;

	void SetAnimation(const char* name, bool loop = true);

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;
private:

	const char* animationName;
	int instanceID = -1;
};

#endif // !__H_COMPONENT_ANIMATION__
