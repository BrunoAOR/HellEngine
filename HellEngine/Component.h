#ifndef __H_COMPONENT__
#define __H_COMPONENT__

#include <string>
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

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() = 0;

public:

	bool isActive = true;
	GameObject* gameObject = nullptr;

protected:

	ComponentType type;
	struct
	{
		int id;
		std::string idLabel;
	} editorInfo;

	static int nextId;
};

#endif // !__H_COMPONENT__
