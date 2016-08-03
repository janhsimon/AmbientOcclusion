#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 inColor;

out vec4 vs_fs_color;

uniform vec2 center;
uniform vec2 scale;

void main(void)
{
	gl_Position = vec4(inPosition.xy * scale + center, 0.0, 1.0);
	vs_fs_color = inColor;
}