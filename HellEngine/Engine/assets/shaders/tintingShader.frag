#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
uniform float intensity;
uniform vec4 tintColor;
void main()
{
color = texture(ourTexture, TexCoord) * (1 - intensity) + tintColor * intensity;
}
