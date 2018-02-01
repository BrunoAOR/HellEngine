#ifndef __H_COMPONENT__
#define __H_COMPONENT__

class GameObject;
enum class ComponentType;

class Component
{
public:

	Component();
	~Component();

	ComponentType getType();

public:

	bool isActive = true;
	GameObject* gameObject;

private:

	ComponentType type;

};

#endif // !__H_COMPONENT__
