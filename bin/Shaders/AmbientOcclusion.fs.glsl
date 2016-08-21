#version 330 core

layout(location = 0) out float outColor;

uniform vec2 screenSize;
uniform vec3 samples[64];
uniform mat4 projectionMatrix;

uniform sampler2D inPosition; // view-space position
uniform sampler2D inNormal; // view-space normal
uniform sampler2D inNoise;

const float RADIUS = 100.0;
const float CONTRAST = 1.2;

void main()
{
	vec2 uv = gl_FragCoord.xy / screenSize;
	vec3 position = texture(inPosition, uv).rgb;
	vec3 normal = texture(inNormal, uv).rgb;
	vec3 randomVec = texture(inNoise, uv * (screenSize / 4.0f)).xyz;
	
	// create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 TBN = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < 64; ++i)
	{
		// get sample position
		vec3 sample = TBN * samples[i]; // From tangent to view-space
		sample = position + sample * RADIUS;

		// project sample position (to sample texture) (to get position on screen/texture)
		vec4 offset = vec4(sample, 1.0);
		offset = projectionMatrix * offset; // from view to clip-space
		offset.xyz /= offset.w; // perspective divide
		offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0

		// get sample depth
		float sampleDepth = -texture(inPosition, offset.xy).w; // get depth value of kernel sample

		// range check and accumulate
		float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(position.z - sampleDepth));
		occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / 64);
	outColor = occlusion * CONTRAST;
}