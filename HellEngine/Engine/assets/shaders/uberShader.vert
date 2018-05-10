#define MAX_BONES 128
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 uvCoord;
layout (location = 4) in ivec4 bone_indices;
layout (location = 5) in vec4 bone_weights;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
	uniform mat4 normal_matrix;
#endif

#if defined(VERTEX_LIGHTING)
	uniform vec3 light_position;
	#if defined(SPECULAR)
		uniform vec3 camera_position;
	#endif
#endif

#if defined(GPU_SKINNING)
	uniform mat4 bones_palette[MAX_BONES];
#endif

out vec2 ourUvCoord;

#if defined(PIXEL_LIGHTING)
	out vec3 worldPosition;
	out vec3 worldNormal;
#elif defined(VERTEX_LIGHTING)
	out float diffuseIntensity;
	#if defined(SPECULAR)
		out float specularIntensity;
	#endif 
#endif

void main()
{
	ourUvCoord = uvCoord;
	vec4 worldPosition4 = vec4(position, 1.0f);
	
	#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
		vec4 worldNormal4 = vec4(normal, 0.0f);
	#endif
	
	#if defined(GPU_SKINNING)
		mat4 skin_transform = bones_palette[bone_indices[0]]*bone_weights[0] + bones_palette[bone_indices[1]]*bone_weights[1] + bones_palette[bone_indices[2]]*bone_weights[2] + bones_palette[bone_indices[3]]*bone_weights[3];
		worldPosition4 = skin_transform * worldPosition4;
		
		#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
			worldNormal4 = skin_transform * worldNormal4;
		#endif
	#endif
	
	worldPosition4 = model_matrix * worldPosition4;
	
	#if defined(PIXEL_LIGHTING) || defined(VERTEX_LIGHTING)
		worldNormal4 = normal_matrix * worldNormal4;
	#endif
	
	#if defined(VERTEX_LIGHTING)
		// DIFFUSE CALC
		vec3 worldPosition = worldPosition4.xyz;
		vec3 VertexToLight = light_position - worldPosition;
		VertexToLight = normalize(VertexToLight);

		vec3 worldNormal = worldNormal4.xyz;

		diffuseIntensity = dot(VertexToLight, worldNormal);
		if (diffuseIntensity < 0)
		{
			diffuseIntensity = 0;
		}

		#if defined(SPECULAR)
			// SPECULAR CALC
			vec3 cameraDir = camera_position - worldPosition;
			vec3 halfVector = normalize(cameraDir + VertexToLight);
			specularIntensity = dot(halfVector, worldNormal);
			if (specularIntensity < 0)
			{
				specularIntensity = 0;
			}

			specularIntensity = pow(specularIntensity, 128.0f);
		#endif
	#endif
	
	#if defined(PIXEL_LIGHTING)
		worldPosition = worldPosition4.xyz;
		worldNormal = worldNormal4.xyz;
	#endif
	
	gl_Position = projection_matrix * view_matrix * worldPosition4;
}
