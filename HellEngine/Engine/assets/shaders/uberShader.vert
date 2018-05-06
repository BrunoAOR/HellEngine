#define MAX_BONES 128
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;
layout (location = 3) in ivec4 bone_indices;
layout (location = 4) in vec4 bone_weights;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform mat4 normal_matrix;

uniform mat4 bones_palette[MAX_BONES];

out vec2 ourUvCoord;
out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
	ourUvCoord = uvCoord;

	vec4 worldPosition4 = vec4(position, 1.0f);
	vec4 worldNormal4 = vec4(normal, 0.0f);
	
	#ifdef GPU_SKINNING
	mat4 skin_transform = bones_palette[bone_indices[0]]*bone_weights[0] + bones_palette[bone_indices[1]]*bone_weights[1] + bones_palette[bone_indices[2]]*bone_weights[2] + bones_palette[bone_indices[3]]*bone_weights[3];
	worldPosition4 = skin_transform * worldPosition4;
	worldNormal4 = skin_transform * worldNormal4;
	#endif
	
	worldPosition4 = model_matrix * worldPosition4;
	worldNormal = (normal_matrix * worldNormal4).xyz;

	gl_Position = projection_matrix * view_matrix * worldPosition4;
}

