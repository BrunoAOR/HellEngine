#ifndef __H_GLOBALS__
#define __H_GLOBALS__

#define LOGGER(format, ...) Log(__FILE__, __LINE__, format, __VA_ARGS__);
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )

/* Deletes a buffer */
#define RELEASE( x )		\
	{						\
       if( x != nullptr )	\
		{					\
			delete x;		\
			x = nullptr;	\
		}					\
	}

/* Deletes an array of buffers */
#define RELEASE_ARRAY( x )	\
	{						\
		if( x != nullptr )	\
		{					\
			delete[] x;		\
			x = nullptr;	\
		}					\
	}

/* Configuration */
#define SCREEN_SIZE 3
#define SCREEN_WIDTH 250
#define SCREEN_HEIGHT 250
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Hell Engine"

#include "Json/json.h"

/* Useful typedefs */
typedef nlohmann::json Json;
typedef unsigned int uint;

void Log(const char file[], int line, const char* format, ...);
Json LoadJson(const char* path);

#endif /* __H_GLOBALS__ */
