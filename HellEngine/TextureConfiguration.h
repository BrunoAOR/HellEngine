#ifndef __H_TEXTURE_CONFIGURATION__
#define __H_TEXTURE_CONFIGURATION__

struct TextureConfiguration
{
public:

	int wrapMode = 0;
	bool mipMaps = false;
	int magnificationMode = 0;
	int minificationMode = 0;

};

#endif // !__H_TEXTURE_CONFIGURATION__
