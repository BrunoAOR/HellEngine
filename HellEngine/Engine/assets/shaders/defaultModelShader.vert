#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
out vec3 ourNormal;
out vec2 ourTexCoord;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
void main()
{
gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0f);
ourNormal = normal;
ourTexCoord = texCoord;
}
