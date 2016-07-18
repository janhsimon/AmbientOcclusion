#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

out vec4 vs_fs_color;
out vec3 vs_fs_normal;
//out vec3 vs_fs_worldPosition;

uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	// bring vertex position from model- to screenspace
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * vec4(inPosition, 1.0);

	// pass on color
	vs_fs_color = inColor;

	// bring normal to worldspace and pass them on
	vs_fs_normal = (worldMatrix * vec4(inNormal, 0.0)).xyz;
	//vs_fs_worldPosition = (worldMatrix * vec4(inPosition, 1.0)).xyz;
}