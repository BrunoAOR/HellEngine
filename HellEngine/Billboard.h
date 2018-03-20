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
	iPoint GetSize() const;
	uint GetWidth() const;
	uint GetHeight() const;

	void SetPosition(const float3& position);
	void SetSize(uint width, uint height);

	const Quad& ComputeQuad(const ComponentCamera* camera);

private:
	float3 position;
	iPoint size;
	Quad quad;
};

#endif
