#ifndef __H_COMPONENT__
#define __H_COMPONENT__

class GameObject;
enum class ComponentType;

class Component
{
public:

	Component();
	virtual ~Component();

	ComponentType GetType();

public:

	bool isActive = true;
	GameObject* gameObject;

	virtual void OnEditor() = 0;

protected:

	ComponentType type;

};

#endif // !__H_COMPONENT__
