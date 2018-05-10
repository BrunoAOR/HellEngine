in vec2 ourUvCoord;

#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
	in mat3 tangentSpace;
#endif
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
uniform sampler2D ourNormal;

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
		vec3 normal = texture2D(ourNormal, ourUvCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		vec3 light = tangentSpace * light_position;
		vec3 position = tangentSpace * worldPosition;
	
		// DIFFUSE CALC	
		vec3 VertexToLight = light - position;
		VertexToLight = normalize(VertexToLight);

		float diffuseIntensity = dot(VertexToLight, normal);
		if (diffuseIntensity < 0)
		{
			diffuseIntensity = 0;
		}

		#if defined(SPECULAR)
			// SPECULAR CALC
			vec3 camera = tangentSpace * camera_position;
			vec3 cameraDir = camera - position;
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
