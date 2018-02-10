#version 330 core

in vec4 gl_FragCoord;
out vec4 color;

void main()
{
float rgb = 1- gl_FragCoord.w * 5;
color = vec4(rgb, rgb, rgb, 1);
}