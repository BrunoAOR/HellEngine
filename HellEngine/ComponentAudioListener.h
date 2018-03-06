#ifndef __H_COMPONENT_AUDIOLISTENER__
#define __H_COMPONENT_AUDIOLISTENER__

#include "Component.h"

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(GameObject * owner);
	~ComponentAudioListener();

	virtual void OnEditor() override;

	/* Returns the maximum number of times that this particular Type of Component can be added to a GameObject */
	virtual int MaxCountInGameObject() override;

private:

};
#endif

