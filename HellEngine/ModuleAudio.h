#ifndef __H_MODULEAUDIO__
#define __H_MODULEAUDIO__

#include <vector>
#include "Module.h"

class ModuleAudio : public Module
{
public:

	ModuleAudio(bool startEnabled = true);
	~ModuleAudio();

	bool Init();
	bool CleanUp();

private:

	
};

#endif /* __H_MODULEAUDIO__ */
