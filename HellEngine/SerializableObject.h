#ifndef __H_SERIALIZABLE_OBJECT__
#define __H_SERIALIZABLE_OBJECT__

#include "globals.h"
#include "MathGeoLib/src/Math/MathTypes.h"
struct Color;

class SerializableObject
{
public:
	SerializableObject(Json* obj);
	~SerializableObject();

	void AddBool(const std::string& key, bool value);
	void AddInt(const std::string& key, int value);
	void Addu32(const std::string& key, u32 value);
	void AddFloat(const std::string& key, float value);
	void AddFloat3(const std::string& key, const float3& value);
	void AddFloat4(const std::string& key, const float4& value);
	void AddColor(const std::string& key, const Color& value);
	void AddString(const std::string& key, const std::string& value);

	bool GetBool(const std::string& key) const;
	int GetInt(const std::string& key) const;
	u32 Getu32(const std::string& key) const;
	float GetFloat(const std::string& key) const;
	float3 GetFloat3(const std::string& key) const;
	float4 GetFloat4(const std::string& key) const;
	Color GetColor(const std::string& key) const;
	std::string GetString(const std::string& key) const;

private:
	Json* jsonObject;
};

#endif // !__H_SERIALIZABLE_OBJECT__
