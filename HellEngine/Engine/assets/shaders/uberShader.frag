in vec2 ourUvCoord;

#if defined(PIXEL_LIGHTING)
	in vec3 worldPosition;
	#if !defined(NORMAL_MAPPING)
		in vec3 worldNormal;
	#endif
	in vec3 lightPosition;
	#if defined(SPECULAR)
		in vec3 cameraPosition;
	#endif
#elif defined(VERTEX_LIGHTING)
	in float diffuseIntensity;
	#if defined(SPECULAR)
		in float specularIntensity;
	#endif
#endif

uniform sampler2D ourTexture;
uniform sampler2D ourNormal;

out vec4 color;

void main()
{
	vec4 baseColor = texture2D(ourTexture, ourUvCoord);
	
	#if defined(PIXEL_LIGHTING)
		vec3 normal;
		#if defined(NORMAL_MAPPING)
			normal = texture2D(ourNormal, ourUvCoord).rgb;
			normal = normalize(normal * 2.0 - 1.0);
		#else
			normal = worldNormal;
		#endif
		
		// DIFFUSE CALC	
		vec3 VertexToLight = lightPosition - worldPosition;
		VertexToLight = normalize(VertexToLight);

		float diffuseIntensity = dot(VertexToLight, normal);
		if (diffuseIntensity < 0)
		{
			diffuseIntensity = 0;
		}

		#if defined(SPECULAR)
			// SPECULAR CALC
			vec3 cameraDir = cameraPosition - worldPosition;
			vec3 halfVector = normalize(cameraDir + VertexToLight);
			float specularIntensity = dot(halfVector, normal);
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
