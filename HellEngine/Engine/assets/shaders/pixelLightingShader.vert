#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uvCoord;

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform mat4 normal_matrix;

out vec2 ourUvCoord;
out vec3 worldPosition;
out vec3 worldNormal;

void main()
{
gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
ourUvCoord = uvCoord;

// DIFFUSE CALC
worldPosition = (model_matrix * vec4(position, 1.0f)).xyz;
worldNormal = (normal_matrix * vec4(normal, 0.0f)).xyz;

}

