in vec2 ourUvCoord;

#if defined(PIXEL_LIGHTING)
	in vec3 worldPosition;
	in vec3 worldNormal;
#elif defined(VERTEX_LIGHTING)
	in float diffuseIntensity;
	in float specularIntensity;
#endif

uniform sampler2D ourTexture;
#if defined(PIXEL_LIGHTING)
	uniform vec3 light_position;
	uniform vec3 camera_position;
#endif

out vec4 color;

void main()
{
	vec4 baseColor = texture2D(ourTexture, ourUvCoord);
	
	#if defined(PIXEL_LIGHTING)
		// DIFFUSE CALC	
		vec3 VertexToLight = light_position - worldPosition;
		VertexToLight = normalize(VertexToLight);

		float diffuseIntensity = dot(VertexToLight, worldNormal);
		if (diffuseIntensity < 0)
		{
			diffuseIntensity = 0;
		}

		// SPECULAR CALC
		vec3 cameraDir = camera_position - worldPosition;
		vec3 halfVector = normalize(cameraDir + VertexToLight);
		float specularIntensity = dot(halfVector, worldNormal);
		if (specularIntensity < 0)
		{
			specularIntensity = 0;
		}

		specularIntensity = pow(specularIntensity, 128.0f);
		
		color = vec4(baseColor.rgb * diffuseIntensity + vec3(specularIntensity,specularIntensity,specularIntensity), baseColor.a);
	#elif defined(VERTEX_LIGHTING)
		color = vec4(baseColor.rgb * diffuseIntensity + specularIntensity, baseColor.a);
	#else
		color = baseColor;
	#endif
}
