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

void Serializer::Test()
{
	std::string path = "C:/Users/bruno/Desktop/Test.json";
	mainJson = LoadJson(path.c_str());

	SerializableObject sObj(&mainJson);

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
