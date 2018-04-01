#include "SerializableArray.h"
#include "SerializableObject.h"

SerializableArray::SerializableArray()
{
}

SerializableArray::SerializableArray(Json * obj)
{
	assert(obj);
	jsonArray = obj;
}

SerializableArray::~SerializableArray()
{
}

SerializableObject SerializableArray::BuildSerializableObject()
{
	assert(jsonArray);
	jsonArray->push_back(Json::object());
	Json& obj = jsonArray->at(jsonArray->size() - 1);
	SerializableObject sObj(&obj);
	return sObj;
}

SerializableObject SerializableArray::GetSerializableObject(uint index) const
{
	assert(index < jsonArray->size());
	Json& jsonObject = jsonArray->at(index);
	return SerializableObject(&jsonObject);
}

uint SerializableArray::Size() const
{
	assert(jsonArray);
	return jsonArray->size();
}
