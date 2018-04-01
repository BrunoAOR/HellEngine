#ifndef __H_COLOR__
#define __H_COLOR__

struct Color
{
public:
	Color() : r(0), g(0), b(0), a(1) {}
	Color(float rValue, float gValue, float bValue) : r(rValue), g(gValue), b(bValue), a(1) {}
	Color(float rValue, float gValue, float bValue, float aValue) : r(rValue), g(gValue), b(bValue), a(aValue) {}

	const float* ptr() const
	{
		return &r;
	}

	float* ptr()
	{
		return &r;
	}

	float r;
	float g;
	float b;
	float a;
};

#endif // !__H_COLOR__
