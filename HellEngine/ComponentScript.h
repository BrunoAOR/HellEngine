#ifndef __H_COMPONENT_SCRIPT__
#define __H_COMPONENT_SCRIPT__

#include "Component.h"
#include "ScriptType.h"

class ComponentScript :
	public Component
{
public:
	ComponentScript(GameObject* owner);
	virtual ~ComponentScript();

	virtual void UpdateScript();

	virtual void OnEditor() override;
	virtual int MaxCountInGameObject() override;

	ScriptType GetScriptType();

protected:
	ScriptType scriptType;
};

#endif
