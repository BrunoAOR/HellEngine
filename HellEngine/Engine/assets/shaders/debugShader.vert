#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

layout (std140) uniform Matrices
{
	uniform mat4 projection_matrix;
	uniform mat4 view_matrix;
};
uniform mat4 model_matrix;

out vec3 ourColor;

void main()
{
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
	ourColor = color;
}
