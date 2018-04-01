#include "Billboard.h"
#include "ComponentCamera.h"

Billboard::Billboard()
{
	position = { 0, 0, 0 };
	size = { 0, 0 };
}

Billboard::~Billboard()
{
}

float3 Billboard::GetPosition() const
{
	return position;
}

fPoint Billboard::GetSize() const
{
	return size;
}

float Billboard::GetWidth() const
{
	return size.x;
}

float Billboard::GetHeight() const
{
	return size.y;
}



void Billboard::SetPosition(const float * pos)
{
	assert(pos);
	position.x = pos[0];
	position.y = pos[1];
	position.z = pos[2];
}

void Billboard::SetPosition(const float3& pos)
{
	position = pos;
}

void Billboard::SetSize(float width, float height)
{
	size.x = width;
	size.y = height;
}

void Billboard::SetWidth(float width)
{
	size.x = width;
}

void Billboard::SetHeight(float height)
{
	size.y = height;
}

const Quad& Billboard::ComputeQuad(const ComponentCamera* camera)
{
	float3 refPos = camera ? camera->GetPosition3() : float3(0.0f, 0.0f, 0.0f);
	float3 up = { 0.0f, 1.0f, 0.0f };
	float3 right = (position - refPos).Cross(up).Normalized();

	quad.vertices[0] = position - (right * (size.x / 2)) - (up * size.y / 2);
	quad.vertices[1] = position + (right * (size.x / 2)) - (up * size.y / 2);
	quad.vertices[2] = position + (right * (size.x / 2)) + (up * size.y / 2);
	quad.vertices[3] = position - (right * (size.x / 2)) + (up * size.y / 2);

	return quad;
}

