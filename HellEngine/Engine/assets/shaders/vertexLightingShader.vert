#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform mat4 normal_matrix;
uniform vec3 light_position;
uniform vec3 camera_position;

out vec2 ourUvCoord;
out float diffuseIntensity;
out float specularIntensity;

void main()
{
gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
ourUvCoord = uvCoord;

// DIFFUSE CALC
vec3 worldPosition = (model_matrix * vec4(position, 1.0f)).xyz;
vec3 VertexToLight = light_position - worldPosition;
VertexToLight = normalize(VertexToLight);

vec3 worldNormal = (normal_matrix * vec4(normal, 0.0f)).xyz;

diffuseIntensity = dot(VertexToLight, worldNormal);
if (diffuseIntensity < 0)
{
	diffuseIntensity = 0;
}

// SPECULAR CALC
vec3 cameraDir = camera_position - worldPosition;
vec3 halfVector = normalize(cameraDir + VertexToLight);
specularIntensity = dot(halfVector, worldNormal);
if (specularIntensity < 0)
{
	specularIntensity = 0;
}

specularIntensity = pow(specularIntensity, 64.0f);


}

