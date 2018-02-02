#ifndef __H_COMPONENT__
#define __H_COMPONENT__

class GameObject;
enum class ComponentType;

class Component
{
public:

	Component(GameObject* owner);
	virtual ~Component();

	ComponentType GetType();

public:

	bool isActive = true;
	GameObject* gameObject = nullptr;

	virtual void OnEditor() = 0;

protected:

	ComponentType type;

};

#endif // !__H_COMPONENT__
