#version 330 core

layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outColor;

in vec3 vs_fs_position; // view-space position
in vec3 vs_fs_normal; // view-space normal
in vec3 vs_fs_color; // diffuse color

uniform vec2 cameraNearFar;

float linearizeDepth(float depth)
// from http://learnopengl.com/#!Advanced-OpenGL/Depth-Testing
{
	float z = depth * 2.0 - 1.0; // [0, 1] -> [-1, 1]
	return (2.0 * cameraNearFar.x * cameraNearFar.y) / (cameraNearFar.y + cameraNearFar.x - z * (cameraNearFar.y - cameraNearFar.x));
}

void main()
{
	outPosition = vec4(vs_fs_position, linearizeDepth(gl_FragCoord.z));
	outNormal = vec4(normalize(vs_fs_normal), 1.0);
	outColor = vec4(vs_fs_color, 1.0);
}