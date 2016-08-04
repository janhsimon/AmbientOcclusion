#version 330

in vec3 vs_fs_color;
in vec3 vs_fs_normal;

layout(location = 0) out vec3 outMRT0;
layout(location = 1) out vec3 outMRT1;

void main()
{
	outMRT0 = vs_fs_color;
	outMRT1 = (vs_fs_normal + 1.0) / 2.0;
}