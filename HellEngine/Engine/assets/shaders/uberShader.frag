in vec2 ourUvCoord;

#if defined(PIXEL_LIGHTING)
	in vec3 worldPosition;
	in vec3 worldNormal;
#elif defined(VERTEX_LIGHTING)
	in float diffuseIntensity;
	#if defined(SPECULAR)
		in float specularIntensity;
	#endif
#endif

uniform sampler2D ourTexture;
#if defined(PIXEL_LIGHTING)
	uniform vec3 light_position;
	#if defined(SPECULAR)
		uniform vec3 camera_position;
	#endif
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

		#if defined(SPECULAR)
			// SPECULAR CALC
			vec3 cameraDir = camera_position - worldPosition;
			vec3 halfVector = normalize(cameraDir + VertexToLight);
			float specularIntensity = dot(halfVector, worldNormal);
			if (specularIntensity < 0)
			{
				specularIntensity = 0;
			}

			specularIntensity = pow(specularIntensity, 128.0f);
		#endif
	#endif
	
	#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
		#if defined(SPECULAR)
			color = vec4(baseColor.rgb * diffuseIntensity + specularIntensity, baseColor.a);
		#else
			color = vec4(baseColor.rgb * diffuseIntensity, baseColor.a);
		#endif
	#else
		color = baseColor;
	#endif
}
