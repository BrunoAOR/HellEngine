#ifndef __H_GAME_OBJECT__
#define __H_GAME_OBJECT__

#include <map>
#include <vector>
#include "MathGeoLib/src/Math/MathTypes.h"

class Component;
class SerializableObject;
enum class ComponentType;
enum class UIElementType;

class GameObject
{
public:

	GameObject(const char* name, GameObject* parentGameObject);
	~GameObject();
	
	void Update();

	void OnEditorInspector();
	void OnEditorRootHierarchy();
	
	GameObject* GetParent();
	bool SetParent(GameObject* newParent);
	bool RemoveChild(GameObject* childToRemove);
    std::vector<GameObject*> GetChildren();

	GameObject* FindByName(const char* lookUpName);

	std::vector<Component*> GetComponents(ComponentType type);
	Component* GetComponent(ComponentType type);
	Component* AddComponent(ComponentType type);
	bool RemoveComponent(Component* component);
	void AddDependingComponent();
	void RemoveDependingComponent();

	bool GetActive() const;
	void SetActive(bool activeState);

	void Save(SerializableObject& obj);
	void Load(const SerializableObject& obj, std::map<u32, Component*>& componentsCreated);
	void LinkComponents(const SerializableObject& obj, const std::map<u32, Component*>& componentsCreated);

	u32 GetUUID() const;
	u32 GetParentUUID() const;

public:

	std::string name;

private:

	bool HasGameObjectInChildrenHierarchy(GameObject* testGameObject);
	bool IsFirstChild();
	bool IsLastChild();
	int GetIndexInSiblings();
	void SwapWithPreviousSibling();
	void SwapWithNextSibling();

	GameObject* AddEmptyChild();
	GameObject* AddCameraChild();
	GameObject* AddLightChild();
	GameObject* AddPlaneChild();
	GameObject* AddCubeChild();
	GameObject* AddSphereChild();
	GameObject* AddUiElement(UIElementType uiElementType);

	bool OnEditorHierarchy();
	void OnEditorHierarchyDragAndDrop();
	bool OnEditorHierarchyRightClick();
	void OnEditorHierarchyCreateMenu();
	bool OnEditorHierarchyLoadModelMenu();

private:

	GameObject* parent = nullptr;
	bool isActive = true;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	static GameObject* hierarchyActiveGameObject;
	bool componentPendingToRemove = false;
	
	u32 uuid = 0;
	u32 parentUuid = 0;
};

#endif // !__H_GAME_OBJECT__
