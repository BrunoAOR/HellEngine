#version 330 core
in vec2 ourUvCoord;
in float diffuseIntensity;
in float specularIntensity;

uniform sampler2D ourTexture;

out vec4 color;

void main()
{
vec4 baseColor = texture2D(ourTexture, ourUvCoord);
//baseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
color = vec4(baseColor.rgb * diffuseIntensity + specularIntensity, baseColor.a);
}
