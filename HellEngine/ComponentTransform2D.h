#ifndef __H_COMPONENT_TRANSFORM_2D__
#define __H_COMPONENT_TRANSFORM_2D__
#include "MathGeoLib/src/Math/float4x4.h"
#include "Component.h"
#include "Point.h"
class GameObject;

class ComponentTransform2D :
	public Component
{
public:
	ComponentTransform2D(GameObject* owner);
	~ComponentTransform2D();

	void RecalculateLocalPos(const ComponentTransform2D* parentTransform2D);

	const fPoint& GetWorldPos() const;
	const fPoint& GetLocalPos() const;
	void SetLocalPos(const fPoint& newPos);
	void SetLocalPos(float x, float y);

	const fPoint& GetSize() const;
	void SetSize(const fPoint& newSize);
	void SetSize(int x, int y);
	void SetSize(float x, float y);

	void UpdateWorldPos(const ComponentTransform2D* parentTransform2D);

	// Inherited via Component
	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

private:
	void UpdateChildrenWorldPos();

private:
	fPoint localPos;
	fPoint size;

	fPoint worldPos;
	bool isVisible = true;
};

#endif // !__H_COMPONENT_TRANSFORM_2D__
