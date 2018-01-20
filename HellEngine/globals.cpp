#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include "Json/json.h"
#include "globals.h"

void Log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list ap;

	/* Construct the string from variable arguments */
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "%s(%d) : %s\n", file, line, tmp_string);
	OutputDebugString(tmp_string2);
}

Json LoadJson(const char* path)
{
	std::ifstream file(path);
	assert(file.good());
	Json json;
	file >> json;
	return json;
}
