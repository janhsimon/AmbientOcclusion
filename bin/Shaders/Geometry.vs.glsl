#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

out vec3 vs_fs_position; // view-space position
out vec3 vs_fs_normal; // view-space normal
out vec3 vs_fs_color; // diffuse color

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	vs_fs_position = (viewMatrix * worldMatrix * vec4(inPosition, 1.0)).xyz;
	vs_fs_normal = (viewMatrix * worldMatrix * vec4(inNormal, 0.0)).xyz;
	vs_fs_color = inColor;
	
	gl_Position = projectionMatrix * vec4(vs_fs_position, 1.0);
}