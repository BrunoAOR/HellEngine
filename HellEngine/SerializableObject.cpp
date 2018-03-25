#include "SerializableObject.h"
#include "SerializableArray.h"
#include "Color.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"

SerializableObject::SerializableObject()
{
}

SerializableObject::SerializableObject(Json* obj)
{
	assert(obj);
	jsonObject = obj;
}

SerializableObject::~SerializableObject()
{
}

SerializableArray SerializableObject::BuildSerializableArray(const std::string& key)
{
	assert(jsonObject);
	(*jsonObject)[key] = Json::array();
	Json& arrayObj = (*jsonObject)[key];
	SerializableArray sArray(&arrayObj);
	return sArray;
}

SerializableObject SerializableObject::BuildSerializableObject(const std::string& key)
{
	assert(jsonObject);
	(*jsonObject)[key] = Json::object();
	Json& obj = (*jsonObject)[key];
	SerializableObject sObj(&obj);
	return sObj;
}

void SerializableObject::AddBool(const std::string & key, bool value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::AddInt(const std::string& key, int value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::Addu32(const std::string & key, u32 value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::AddFloat(const std::string& key, float value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::AddFloat3(const std::string & key, const float3& value) 
{
	Json jsonObj;
	jsonObj["x"] = value.x;
	jsonObj["y"] = value.y;
	jsonObj["z"] = value.z;
	(*jsonObject)[key] = jsonObj;
}

void SerializableObject::AddFloat4(const std::string & key, const float4& value)
{
	Json jsonObj;
	jsonObj["x"] = value.x;
	jsonObj["y"] = value.y;
	jsonObj["z"] = value.z;
	jsonObj["w"] = value.w;
	(*jsonObject)[key] = jsonObj;
}

void SerializableObject::AddColor(const std::string& key, const Color & value)
{
	Json jsonObj;
	jsonObj["r"] = value.r;
	jsonObj["g"] = value.g;
	jsonObj["b"] = value.b;
	jsonObj["a"] = value.a;
	(*jsonObject)[key] = jsonObj;
}

void SerializableObject::AddString(const std::string& key, const std::string& value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::AddVectorInt(const std::string& key, const std::vector<int>& value)
{
	(*jsonObject)[key] = value;
}

void SerializableObject::AddVectorString(const std::string& key, const std::vector<std::string>& value)
{
	(*jsonObject)[key] = value;
}

SerializableArray SerializableObject::GetSerializableArray(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		Json& jsonArray = (*jsonObject)[key];
		return SerializableArray(&jsonArray);
	}
	assert(false);
	return SerializableArray();
}

SerializableObject SerializableObject::GetSerializableObject(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		Json& jsonObj = (*jsonObject)[key];
		return SerializableObject(&jsonObj);
	}
	assert(false);
	return SerializableObject();
}

bool SerializableObject::GetBool(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return false;
}

int SerializableObject::GetInt(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return 0;
}

u32 SerializableObject::Getu32(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return 0;
}

float SerializableObject::GetFloat(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return 0.0f;
}

float3 SerializableObject::GetFloat3(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		float3 f3;
		Json& jsonObj = (*jsonObject)[key];
		f3.x = jsonObj["x"];
		f3.y = jsonObj["y"];
		f3.z = jsonObj["z"];
		return f3;
	}
	assert(false);
	return float3();
}

float4 SerializableObject::GetFloat4(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		float4 f4;
		Json& jsonObj = (*jsonObject)[key];
		f4.x = jsonObj["x"];
		f4.y = jsonObj["y"];
		f4.z = jsonObj["z"];
		f4.w = jsonObj["w"];
		return f4;
	}
	assert(false);
	return float4();
}

Color SerializableObject::GetColor(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		Color c;
		Json& jsonObj = (*jsonObject)[key];
		c.r = jsonObj["r"];
		c.g = jsonObj["g"];
		c.b = jsonObj["b"];
		c.a = jsonObj["a"];
		return c;
	}
	assert(false);
	return Color();
}

std::string SerializableObject::GetString(const std::string& key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return std::string();
}

std::vector<int> SerializableObject::GetVectorInt(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return std::vector<int>();
}

std::vector<std::string> SerializableObject::GetVectorString(const std::string & key) const
{
	if (jsonObject->count(key))
	{
		return (*jsonObject)[key];
	}
	assert(false);
	return std::vector<std::string>();
}
