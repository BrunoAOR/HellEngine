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

	void SetAnimation();
	void UnsetAnimation();

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

private:

	int instanceID = -1;
	char animationName[256] = "";
	bool loop = true;
	int blendTime = 2000;
};

#endif // !__H_COMPONENT_ANIMATION__
