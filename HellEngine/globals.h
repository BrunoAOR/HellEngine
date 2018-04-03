#ifndef __H_GLOBALS__
#define __H_GLOBALS__

#define LOGGER(format, ...) Logger(__FILE__, __LINE__, format, __VA_ARGS__);
#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#define MIN( a, b ) ( ((a) < (b)) ? (a) : (b) )
#define SP_ARR_2(x) x[0], x[1]
#define SP_ARR_3(x) x[0], x[1], x[2]

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
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768
#define FULLSCREEN false
#define VSYNC true
#define TITLE "Hell Engine"

#include <string>
#include "Json/json.h"

namespace math {
	class float4x4;
	class float3;
	class float4;
	class Quat;
}
typedef math::float4x4 float4x4;
typedef math::float3 float3;
typedef math::float4 float4;
typedef math::Quat Quat;

/* Useful typedefs */
typedef nlohmann::json Json;
typedef unsigned int uint;

extern bool DEBUG_MODE;

void Logger(const char file[], int line, const char* format, ...);
bool SaveJson(const Json& json, const std::string& fileName);
Json LoadJson(const char* path);
bool LoadTextFile(const std::string& path, std::string& outputString);
bool SaveTextFile(const std::string& path, const std::string& content);
bool DeleteFileByPath(const char* path);
bool IsEmptyString(const char* charString);
void DecomposeMatrix(const float4x4& openGlStyleMatrix, float3& position, Quat& rotation, float3& scale);
void DecomposeMatrixPosition(const float4x4& openGlStyleMatrix, float3& position);

#endif /* __H_GLOBALS__ */
