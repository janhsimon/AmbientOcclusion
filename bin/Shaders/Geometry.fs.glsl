#version 330

in vec3 vs_fs_color;
in vec3 vs_fs_normal;
in vec3 vs_fs_posWS;

layout(location = 0) out vec4 outMRT0;
layout(location = 1) out vec4 outMRT1;
layout(location = 2) out vec4 outMRT2;

uniform vec3 eyePosition;

void main()
{
	outMRT0 = vec4(vs_fs_color, length(vs_fs_posWS - eyePosition));
	outMRT1 = vec4((vs_fs_normal + 1.0) / 2.0, 1.0);
	outMRT2 = vec4(vs_fs_posWS, 1.0);
}