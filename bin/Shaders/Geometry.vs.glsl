#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;

out vec3 vs_fs_color;
out vec3 vs_fs_normal;
out vec3 vs_fs_posWS;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	vec4 posWS = worldMatrix * vec4(inPosition, 1.0);
	vs_fs_posWS = posWS.xyz;
	
	vs_fs_normal = (worldMatrix * vec4(inNormal, 0.0)).xyz;

	vs_fs_color = inColor;

	gl_Position = projectionMatrix * viewMatrix * posWS;
}