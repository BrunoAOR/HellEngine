#include "SerializableObject.h"
#include "Color.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"

SerializableObject::SerializableObject(Json* obj)
{
	assert(obj);
	jsonObject = obj;
}

SerializableObject::~SerializableObject()
{
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

void SerializableObject::AddString(const std::string& key, const std::string & value)
{
	(*jsonObject)[key] = value;
}

bool SerializableObject::GetBool(const std::string & key) const
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
