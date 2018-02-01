#ifndef __H_GAME_OBJECT__
#define __H_GAME_OBJECT__

#include <vector>
class Component;
enum class ComponentType;

class GameObject
{
public:

	GameObject(const char* name, GameObject* parent);
	~GameObject();
	
	GameObject* GetParent();
	bool SetParent(GameObject* newParent);

	std::vector<Component*> GetComponents(ComponentType type);
	Component* AddComponent(ComponentType type);
	bool RemoveComponent(Component* component);

public:

	bool isActive = true;
	std::string name;

private:

	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	std::vector<Component*> components;

};

#endif // !__H_GAME_OBJECT__
