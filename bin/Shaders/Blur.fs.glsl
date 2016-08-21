#version 330 core

layout(location = 0) out float outColor;

uniform vec2 screenSize;

uniform sampler2D inAmbientOcclusion;

const int BLUR_SIZE = 4; // use size of noise texture

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec2 texelSize = 1.0 / vec2(textureSize(inAmbientOcclusion, 0));

	float result = 0.0;
	for (int x = 0; x < BLUR_SIZE; ++x)
	{
		for (int y = 0; y < BLUR_SIZE; ++y)
		{
			vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
			result += texture(inAmbientOcclusion, uv + offset).r;
		}
	}

	outColor = result / float(BLUR_SIZE * BLUR_SIZE);
}