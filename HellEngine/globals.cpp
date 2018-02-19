#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <fstream>
#include <stdio.h>
#include <windows.h>
#include "Json/json.h"
#include "MathGeoLib/src/Math/float4x4.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/Quat.h"
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

void DecomposeMatrix(const float4x4& openGlStyleMatrix, float3& position, Quat& rotation, float3& scale)
{
	/* Transpose to get the matrix in a shape when the bottom line is 0 0 0 1 */
	float4x4 nonOpenGlStyleMatrix(openGlStyleMatrix);
	nonOpenGlStyleMatrix.Transpose();

	/* Extract the Translation, Scale and Rotation */
	/*
	Matrix shape

	a  b  c  d
	e  f  g  h
	i  j  k  l
	0  0  0  1
	*/

	/* Extract translation */
	/* x from d, y from h, z from l*/
	position.x = nonOpenGlStyleMatrix[0][3];
	position.y = nonOpenGlStyleMatrix[1][3];
	position.z = nonOpenGlStyleMatrix[2][3];
	/* Zero out extracted positions */
	nonOpenGlStyleMatrix[0][3] = 0;
	nonOpenGlStyleMatrix[1][3] = 0;
	nonOpenGlStyleMatrix[2][3] = 0;

	/* Extract scale */
	/* x from aei.Length, y from bfj.Length, z from cgh.Length */
	scale.x = (float3(nonOpenGlStyleMatrix[0][0], nonOpenGlStyleMatrix[1][0], nonOpenGlStyleMatrix[2][0])).Length();
	scale.y = (float3(nonOpenGlStyleMatrix[0][1], nonOpenGlStyleMatrix[1][1], nonOpenGlStyleMatrix[2][1])).Length();
	scale.z = (float3(nonOpenGlStyleMatrix[0][2], nonOpenGlStyleMatrix[1][2], nonOpenGlStyleMatrix[2][2])).Length();

	/* Extract rotation */
	/* Divide each column by the corresponding scale value */
	nonOpenGlStyleMatrix[0][0] /= scale.x;
	nonOpenGlStyleMatrix[1][0] /= scale.x;
	nonOpenGlStyleMatrix[2][0] /= scale.x;
	nonOpenGlStyleMatrix[0][1] /= scale.y;
	nonOpenGlStyleMatrix[1][1] /= scale.y;
	nonOpenGlStyleMatrix[2][1] /= scale.y;
	nonOpenGlStyleMatrix[0][2] /= scale.z;
	nonOpenGlStyleMatrix[1][2] /= scale.z;
	nonOpenGlStyleMatrix[2][2] /= scale.z;

	/* Then turn into a Quat */
	rotation = Quat(nonOpenGlStyleMatrix);
}
