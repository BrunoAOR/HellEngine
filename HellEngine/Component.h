#ifndef __H_COMPONENT__
#define __H_COMPONENT__

class GameObject;
enum class ComponentType;

class Component
{
public:

	Component();
	~Component();

	ComponentType GetType();

public:

	bool isActive = true;
	GameObject* gameObject;

protected:

	ComponentType type;

};

#endif // !__H_COMPONENT__
