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

void Serializer::Test()
{
	std::string path = "C:/Users/bruno/Desktop/Test.json";
	mainJson = LoadJson(path.c_str());

	SerializableObject sObj(&mainJson);

	SerializableArray someArray = sObj.GetSerializableArray("my Array");

	for (uint i = 0; i < someArray.Size(); ++i)
	{
		SerializableObject someObject = someArray.GetSerializableObject(i);
		LOGGER("Got %i in key Num", someObject.GetInt("Num"));
	}


	Color red(1, 0, 0, 1);
	sObj.AddColor("red", red);
	sObj.AddFloat("float", 1.5f);
	sObj.AddFloat("gato", 1.3f);

	SerializableArray sArray = sObj.BuildSerializableArray("my Array");
	for (int i = 0; i < 3; ++i)
	{
		SerializableObject iObj = sArray.BuildSerializableObject();
		iObj.AddInt("Num", i);
	}

	std::vector<int> numVec{1, 2, 3};
	sObj.AddVectorInt("numVec", numVec);

	std::vector<std::string> stringVec{ "a", "b", "c" };
	sObj.AddVectorString("stringVec", stringVec);

	SaveJson(mainJson, "C:/Users/bruno/Desktop/Test.json");
}
