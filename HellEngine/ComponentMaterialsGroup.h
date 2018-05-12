#ifndef __H_COMPONENT_MATERIALS_GROUP__
#define __H_COMPONENT_MATERIALS_GROUP__

#include "Component.h"
class ComponentMaterial;
enum class ShaderOptions : unsigned int;

class ComponentMaterialsGroup :
	public Component
{
public:
	ComponentMaterialsGroup(GameObject* owner);
	~ComponentMaterialsGroup();

	// Inherited via Component
	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

	/* Attemps to apply all of the material setup */
	bool Apply();

	/* Applies the default material configuration */
	void SetDefaultMaterialConfiguration();

private:
	bool ApplyConfig(ComponentMaterial* material);

private:
	bool isValid = false;

	char vertexShaderPath[256] = "";
	char fragmentShaderPath[256] = "";
	char diffusePath[256] = "";
	char normalPath[256] = "";
	char specularPath[256] = "";
	char shaderDataPath[256] = "";
};

#endif // !__H_COMPONENT_MATERIALS_GROUP__
