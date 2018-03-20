#include "Serializer.h"
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
	
	SaveJson(mainJson, "C:/Users/bruno/Desktop/Test.json");
}

void Serializer::Start()
{
}

std::string Serializer::End()
{
	return std::string();
}

void Serializer::OpenArray()
{

}

void Serializer::CloseArray()
{
}
