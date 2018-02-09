#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include "Json/json.h"
#include "globals.h"

bool DEBUG_MODE = true;

void Logger(const char file[], int line, const char* format, ...)
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

bool LoadTextFile(const std::string& path, std::string& outputString)
{
	outputString = "";
	std::string line;
	std::ifstream inFile(path);
	if (inFile.good())
	{
		while (getline(inFile, line))
		{
			outputString += line + '\n';
		}
		inFile.close();
		return true;
	}
	
	return false;
}

bool SaveTextFile(const std::string& path, const std::string& content)
{
	std::ofstream outFile(path);
	if (outFile.good())
	{
		outFile << content;
		return true;
	}
	return false;
}

bool IsEmptyString(const char * charString)
{
	return (charString != nullptr && charString[0] == '\0');
}
