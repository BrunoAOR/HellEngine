#ifndef __H_COMPONENT__
#define __H_COMPONENT__

#include <string>
#include "MathGeoLib/src/Math/MathTypes.h"

class GameObject;
class SerializableObject;
enum class ComponentType;

class Component
{
public:

	Component(GameObject* owner);
	virtual ~Component();

	ComponentType GetType();

	virtual void Update();

	bool GetActive() const;
	void SetActive(bool activeState);

	virtual void OnEditor() = 0;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() = 0;

	virtual void Save(SerializableObject& obj) const;

	virtual void Load(const SerializableObject& obj);

	bool OnEditorDeleteComponent();

public:

	GameObject* gameObject = nullptr;
	bool toRemove = false;

	u32 uuid;

protected:

	ComponentType type;
	bool isActive = true;
	
	struct
	{
		int id;
		std::string idLabel;
	} editorInfo;

	static int nextId;

};

#endif // !__H_COMPONENT__
