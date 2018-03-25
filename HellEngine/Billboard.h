#ifndef __H_BILLBOARD__
#define __H_BILLBOARD__

#include "MathGeoLib/src/Math/float3.h"
#include "Point.h"
#include "globals.h"

class ComponentCamera;

struct Quad
{
	float3 vertices[4];
};

class Billboard {

public:
	Billboard();
	~Billboard();

	float3 GetPosition() const;
	fPoint GetSize() const;
	float GetWidth() const;
	float GetHeight() const;

	void SetPosition(const float* position);
	void SetPosition(const float3& position);
	void SetSize(float width, float height);
	void SetWidth(float width);
	void SetHeight(float height);

	const Quad& ComputeQuad(const ComponentCamera* camera);

private:
	float3 position;
	fPoint size;
	Quad quad;
};

#endif
