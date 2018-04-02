#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

out vec2 ourTexCoord;

void main()
{
	gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
	ourTexCoord = texCoord;
}