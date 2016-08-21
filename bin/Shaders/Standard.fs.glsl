#version 330 core

layout(location = 0) out vec3 outColor;

uniform vec2 screenSize;

uniform sampler2D inTexture;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	float color = texture(inTexture, uv).r;
	outColor = vec3(color, color, color);
}