#ifndef __H_COMPONENT_LIGHT__
#define __H_COMPONENT_LIGHT__

#include "MathGeoLib/src/Math/float3.h"
#include "Component.h"
class ComponentTransform;

class ComponentLight :
	public Component
{
public:
	ComponentLight(GameObject* owner);
	virtual ~ComponentLight() override;
	
	// Inherited via Component
	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

	virtual void Save(SerializableObject& obj) const override;
	virtual void Load(const SerializableObject& obj) override;

	const float3* GetPosition();
	void SetAsActiveLight();

public:
	bool isActiveLight = false;

private:
	ComponentTransform* transform = nullptr;
	
};

#endif // !__H_COMPONENT_LIGHT__
