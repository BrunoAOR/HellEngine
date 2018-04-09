#ifndef __H_HELLO_WORLD__
#define __H_HELLO_WORLD__

#include "ComponentScript.h"

class HelloWorld :
	public ComponentScript
{
public:

	HelloWorld(GameObject* owner);
	virtual ~HelloWorld();

	virtual void UpdateScript() override;
};

#endif