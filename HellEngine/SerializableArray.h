#ifndef __H_SERIALIZABLE_ARRAY__
#define __H_SERIALIZABLE_ARRAY__
#include "globals.h"
class SerializableObject;

class SerializableArray
{
public:
	SerializableArray();
	SerializableArray(Json* obj);
	~SerializableArray();

	SerializableObject BuildSerializableObject();
	SerializableObject GetSerializableObject(uint index);
	uint Size() const;

private:
	Json* jsonArray = nullptr;
};

#endif // !__H_SERIALIZABLE_ARRAY__
