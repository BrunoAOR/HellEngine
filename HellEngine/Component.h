#ifndef __H_COMPONENT__
#define __H_COMPONENT__

#include "dllControl.h"

#include <map>
#include <string>
#include "MathGeoLib/src/Math/MathTypes.h"

class GameObject;
class SerializableObject;
enum class ComponentType;

class ENGINE_API Component
{
public:

	Component(GameObject* owner);
	virtual ~Component();

	ComponentType GetType();

	virtual void Update();

	bool GetActive() const;
	void SetActive(bool activeState);

	virtual void OnEditor() = 0;

	u32 GetUUID() const;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() = 0;

	virtual void Save(SerializableObject& obj) const;
	virtual void Load(const SerializableObject& obj);
	virtual void LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated);

	bool OnEditorDeleteComponent();

public:

	GameObject* gameObject = nullptr;
	bool toRemove = false;

protected:

	u32 uuid;
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
