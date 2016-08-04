#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

out vec3 vs_fs_color;
out vec3 vs_fs_normal;

uniform mat4 worldMatrix;
uniform mat4 viewProjectionMatrix;

void main(void)
{
	gl_Position = viewProjectionMatrix * worldMatrix * vec4(inPosition, 1.0);
	vs_fs_normal = (worldMatrix * vec4(inNormal, 0.0)).xyz;
	vs_fs_color = inColor;
}