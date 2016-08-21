#version 330 core

layout(location = 0) in vec3 inPosition;

out vec3 vs_fs_lightDirection; // view-space light direction

uniform vec3 lightDirection;
uniform mat4 viewMatrix;

void main(void)
{
	vs_fs_lightDirection = (viewMatrix * vec4(lightDirection, 0.0)).xyz;

	gl_Position = vec4(inPosition.xy, 0.0, 0.0);
}