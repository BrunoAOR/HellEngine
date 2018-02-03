#ifndef __H_GLOBALS__
#define __H_GLOBALS__

#define LOGGER(format, ...) Logger(__FILE__, __LINE__, format, __VA_ARGS__);
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
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Hell Engine"

#include <string>
#include "Json/json.h"

/* Useful typedefs */
typedef nlohmann::json Json;
typedef unsigned int uint;

void Logger(const char file[], int line, const char* format, ...);
Json LoadJson(const char* path);
bool LoadTextFile(const std::string& path, std::string& outputString);
bool SaveTextFile(const std::string& path, const std::string& content);

#endif /* __H_GLOBALS__ */
