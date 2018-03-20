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

	void Test();

private:
	Json mainJson;
};

#endif // !__H_SERIALIZER__
