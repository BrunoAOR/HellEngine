#ifndef _JSON_SERIALIZER_H_
#define _JSON_SERIALIZER_H_

#include "MathGeoLib/src/Math/Quat.h"
#include "MathGeoLib/src/Math/float3.h"
#include "MathGeoLib/src/Math/float4.h"
#include "Json/json.h"
#include "MathGeoLib/src/Geometry/Frustum.h"
#include <set>
#include <experimental/filesystem>

void to_json(nlohmann::json& j, const float2& v);
void from_json(const nlohmann::json& j, float2& v);

void to_json(nlohmann::json& j, const float3& v);
void from_json(const nlohmann::json& j, float3& v);

void to_json(nlohmann::json& j, const float4& v);
void from_json(const nlohmann::json& j, float4& v);

void to_json(nlohmann::json& j, const Quat& v);
void from_json(const nlohmann::json& j, Quat& v);

void to_json(nlohmann::json& j, const Frustum& v);
void from_json(const nlohmann::json& j, Frustum& v);

#endif // !_JSON_SERIALIZER_H_