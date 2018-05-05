#version 330 core
in vec2 ourUvCoord;
in vec3 worldPosition;
in vec3 worldNormal;

uniform sampler2D ourTexture;
uniform vec3 light_position;
uniform vec3 camera_position;

out vec4 color;

void main()
{
vec3 VertexToLight = light_position - worldPosition;
VertexToLight = normalize(VertexToLight);

float diffuseIntensity = dot(VertexToLight, worldNormal);
if (diffuseIntensity < 0)
{
	diffuseIntensity = 0;
}

// SPECULAR CALC
vec3 cameraDir = camera_position - worldPosition;
vec3 halfVector = normalize(cameraDir + VertexToLight);
float specularIntensity = dot(halfVector, worldNormal);
if (specularIntensity < 0)
{
	specularIntensity = 0;
}

specularIntensity = pow(specularIntensity, 128.0f);

vec4 baseColor = texture2D(ourTexture, ourUvCoord);
//baseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
color = vec4(baseColor.rgb * diffuseIntensity + vec3(specularIntensity,specularIntensity,specularIntensity), baseColor.a);
}
