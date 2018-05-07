#ifndef __H_SHADER_OPTIONS__
#define __H_SHADER_OPTIONS__

enum class ShaderOptions : unsigned int
{
	NONE = 0,
	DEFAULT = 1 << 0,
	VERTEX_LIGHTING = 1 << 1,
	PIXEL_LIGHTING = 1 << 2,
	SPECULAR = 1 << 3,
	GPU_SKINNING = 1 << 4
};

inline constexpr ShaderOptions operator &(ShaderOptions shaderOption1, ShaderOptions shaderOption2)
{
	return static_cast<ShaderOptions>
		(static_cast<unsigned int>(shaderOption1) & static_cast<unsigned int>(shaderOption2));
}

inline constexpr ShaderOptions operator |(ShaderOptions shaderOption1, ShaderOptions shaderOption2)
{
	return static_cast<ShaderOptions>
		(static_cast<unsigned int>(shaderOption1) | static_cast<unsigned int>(shaderOption2));
}

inline constexpr ShaderOptions operator ^(ShaderOptions shaderOption1, ShaderOptions shaderOption2)
{
	return static_cast<ShaderOptions>
		(static_cast<unsigned int>(shaderOption1) ^ static_cast<unsigned int>(shaderOption2));
}

inline constexpr ShaderOptions operator ~(ShaderOptions shaderOption)
{
	return static_cast<ShaderOptions>(~static_cast<unsigned int>(shaderOption));
}

inline ShaderOptions& operator &=(ShaderOptions& shaderOption1, ShaderOptions shaderOption2)
{
	shaderOption1 = shaderOption1 & shaderOption2;
	return shaderOption1;
}

inline ShaderOptions& operator |=(ShaderOptions& shaderOption1, ShaderOptions shaderOption2)
{
	shaderOption1 = shaderOption1 | shaderOption2;
	return shaderOption1;
}

inline ShaderOptions& operator ^=(ShaderOptions& shaderOption1, ShaderOptions shaderOption2)
{
	shaderOption1 = shaderOption1 ^ shaderOption2;
	return shaderOption1;
}

#endif // !__H_SHADER_OPTIONS__
