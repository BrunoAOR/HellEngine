#ifndef __H_TEXTURE_INFO__
#define __H_TEXTURE_INFO__

#include "globals.h"

struct TextureInfo
{
public:

	void Zero()
	{
		width = 0;
		height = 0;
		bytesPerPixel = 0;
	}

public:

	uint width = 0;
	uint height = 0;
	uint bytesPerPixel = 0;
};

#endif // !__H_TEXTURE_INFO__
