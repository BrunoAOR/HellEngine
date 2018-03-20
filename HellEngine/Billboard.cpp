#include "Billboard.h"
#include "ComponentCamera.h"

Billboard::Billboard()
{
}

Billboard::~Billboard()
{
}

float3 Billboard::GetPosition() const
{
	return position;
}

iPoint Billboard::GetSize() const
{
	return size;
}

uint Billboard::GetWidth() const
{
	return size.x;
}

uint Billboard::GetHeight() const
{
	return size.y;
}

void Billboard::SetPosition(const float3& pos)
{
	position = pos;
}

void Billboard::SetSize(uint width, uint height)
{
	size.x = width;
	size.y = height;
}

const Quad& Billboard::ComputeQuad(const ComponentCamera* camera)
{
	float3 up = { 0.0f, 1.0f, 0.0f };
	float3 right = (position - camera->GetPosition3()).Cross(up).Normalized();
	float3 normal = right.Cross(up).Normalized();

	quad.vertices[0] = position - (right * ((float) size.x / 2)) - (up * (float) size.y / 2);
	quad.vertices[1] = position + (right * ((float) size.x / 2)) - (up * (float) size.y / 2);
	quad.vertices[2] = position + (right * ((float) size.x / 2)) + (up * (float) size.y / 2);
	quad.vertices[3] = position - (right * ((float) size.x / 2)) + (up * (float) size.y / 2);

	return quad;
}

