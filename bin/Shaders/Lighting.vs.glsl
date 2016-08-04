#version 330

layout(location = 0) in vec3 inPosition;

void main(void)
{
	gl_Position = vec4(inPosition.xy, 0.0, 0.0);
}