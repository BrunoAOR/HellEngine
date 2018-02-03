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

	virtual void Update();

	virtual void OnEditor() = 0;


public:

	bool isActive = true;
	GameObject* gameObject = nullptr;

protected:

	ComponentType type;

};

#endif // !__H_COMPONENT__
