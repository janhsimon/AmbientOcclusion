#version 330 core

layout(location = 0) out vec3 outColor;

in vec3 vs_fs_lightDirection; // view-space light direction

uniform vec2 screenSize;
uniform vec3 lightColor;

uniform sampler2D inNormal; // view-space normal
uniform sampler2D inColor; // diffuse color

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec3 normal = texture(inNormal, uv).rgb;
	vec3 color = texture(inColor, uv).rgb;
	
	vec3 diffuseLight = dot(normal, -vs_fs_lightDirection) * lightColor;
	outColor = clamp(color * 2.4 * diffuseLight, 0.0, 1.0);
}