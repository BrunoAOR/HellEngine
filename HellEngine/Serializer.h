#ifndef __H_SERIALIZER__
#define __H_SERIALIZER__

#include <string>
#include <stack>
#include "globals.h"
#include "SerializableObject.h"

class Serializer
{
public:
	Serializer();
	~Serializer();

	SerializableObject GetEmptySerializableObject();
	bool Save(const std::string& fileName);
	SerializableObject Load(const std::string& fileName);

private:
	Json mainJson;
};

#endif // !__H_SERIALIZER__
