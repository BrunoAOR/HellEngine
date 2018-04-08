#ifndef __H_JUMP__
#define __H_JUMP__

#include "ComponentScript.h"

class Jump :
	public ComponentScript
{
public:

	Jump(GameObject* owner);
	virtual ~Jump();

	virtual void UpdateScript() override;

private:
	unsigned int elapsedTime = 0;
};

#endif