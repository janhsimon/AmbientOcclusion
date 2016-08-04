#version 330

layout(location = 0) out vec3 outColor;

uniform vec2 screenSize;

uniform sampler2D inGBufferMRT0;
uniform sampler2D inGBufferMRT1;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	
	vec3 color = texture(inGBufferMRT0, uv).rgb;

	vec3 normal = texture(inGBufferMRT1, uv).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	
	float diffuseTerm = dot(normal, vec3(0.2, 1.0, 0.2));
	outColor = color * diffuseTerm;
}