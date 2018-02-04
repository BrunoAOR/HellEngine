#ifndef __H_GAME_OBJECT__
#define __H_GAME_OBJECT__

#include <vector>
class Component;
enum class ComponentType;

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

	std::vector<Component*> GetComponents(ComponentType type);
	Component* AddComponent(ComponentType type);
	bool RemoveComponent(Component* component);

public:

	bool isActive = true;
	std::string name;

private:

	bool HasGameObjectInChildrenHierarchy(GameObject* testGameObject);
	bool IsFirstChild();
	bool IsLastChild();

	void OnEditorHierarchy();
	void OnEditorHierarchyDragAndDrop();
	void OnEditorHierarchyRightClick();

private:

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

	static GameObject* hierarchyActiveGameObject;
};

#endif // !__H_GAME_OBJECT__
