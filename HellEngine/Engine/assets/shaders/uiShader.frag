#version 330 core

uniform sampler2D ourTexture;
uniform vec4 tintColor;
in vec2 ourTexCoord;

out vec4 color;

void main()
{
	color = texture(ourTexture, ourTexCoord) * tintColor;
}
