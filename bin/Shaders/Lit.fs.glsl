#version 330

in vec4 vs_fs_color;
in vec3 vs_fs_normal;

layout (location = 0) out vec4 outColor;

void main()
{
	float diffuseTerm = clamp(dot(vec3(1.0, 1.0, -1.0), vs_fs_normal), 0.0, 1.0) + 0.25;
	outColor = vec4(vs_fs_color.rgb * diffuseTerm, vs_fs_color.a);
}