#version 330 core

uniform sampler2D ourTexture;

in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

void main()
{
	color = texture2D(ourTexture, TexCoord);
}
