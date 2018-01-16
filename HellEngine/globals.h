#ifndef __H_GLOBALS__
#define __H_GLOBALS__

#include "Json/json.h"
using Json = nlohmann::json;

#define LOGGER(format, ...) Log(__FILE__, __LINE__, format, __VA_ARGS__);
void Log(const char file[], int line, const char* format, ...);

Json LoadJson(const char* path);

#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )

/* Useful typedefs */
typedef unsigned int uint;

/* Deletes a buffer */
#define RELEASE( x ) \
    {									  \
       if( x != nullptr )   \
       {						      \
         delete x;                  \
	     x = nullptr;             \
       }                      \
    }

/* Deletes an array of buffers */
#define RELEASE_ARRAY( x ) \
	{                              \
       if( x != nullptr )              \
       {                            \
           delete[] x;                \
	       x = nullptr;                    \
		 }                            \
                              \
	 }

/* Configuration */
#define SCREEN_SIZE 3
#define SCREEN_WIDTH 384
#define SCREEN_HEIGHT 240
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Hell Engine"

#endif /* __H_GLOBALS__ */
