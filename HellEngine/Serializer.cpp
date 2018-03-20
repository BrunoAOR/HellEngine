#include "Serializer.h"
#include "SerializableArray.h"
#include "SerializableObject.h"
#include "Color.h"

Serializer::Serializer()
{
}


Serializer::~Serializer()
{
}

SerializableObject Serializer::GetEmptySerializableObject()
{
	mainJson.clear();
	return SerializableObject(&mainJson);
}

bool Serializer::Save(const std::string& fileName)
{
	return SaveJson(mainJson, fileName);
}

SerializableObject Serializer::Load(const std::string& fileName)
{
	mainJson.clear();
	mainJson = LoadJson(fileName.c_str());
	return SerializableObject(&mainJson);
}
