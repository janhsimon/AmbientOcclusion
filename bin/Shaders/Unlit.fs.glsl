#version 330

in vec4 vs_fs_color;

layout (location = 0) out vec4 outColor;

void main()
{
	outColor = vs_fs_color;
}