#version 330

layout(location = 0) out vec3 outColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec2 screenSize;
uniform vec3 eyePosition;
uniform vec3 randomBallSamples[32];

uniform sampler2D inGBufferMRT0;
uniform sampler2D inGBufferMRT2;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec4 gBufferMRT0Sample = texture(inGBufferMRT0, uv);
	vec4 gBufferMRT2Sample = texture(inGBufferMRT2, uv);
	
	vec3 worldPosition = gBufferMRT2Sample.rgb;

	int numVisiblePoints = 0;
	for (int i = 0; i < 32; ++i)
	{
		vec3 spherePoint = worldPosition + randomBallSamples[i] * /*radius*/10.0;

		vec4 p = projectionMatrix * viewMatrix * vec4(spherePoint, 1.0);
		vec2 puv = p.xy / p.w;
		puv = puv * 0.5 + 0.5;
		float depth = texture(inGBufferMRT0, puv).a;

		float len = length(spherePoint - eyePosition);
		if (len <= depth)
		{
			vec3 v = normalize(spherePoint - eyePosition) * depth;

			if (abs(depth - len) <= /*radius * 2.0*/20.0)
			{
				++numVisiblePoints;
			}
		}
	}

	float ambientOcclusionTerm = numVisiblePoints / 32.0;
	outColor = vec3(1.0, 1.0, 1.0) * ambientOcclusionTerm * 3.0;
}