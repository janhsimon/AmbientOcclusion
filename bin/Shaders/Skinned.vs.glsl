#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inBoneWeight;
layout(location = 4) in ivec4 inBoneIndex;

out vec4 vs_fs_color;
out vec3 vs_fs_normal;

uniform mat4 worldMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 boneMatrix[64];
uniform mat3 boneMatrixIT[64];

void main()
{
	mat4 boneTransform = boneMatrix[inBoneIndex.x] * inBoneWeight.x;
	boneTransform += boneMatrix[inBoneIndex.y] * inBoneWeight.y;
	boneTransform += boneMatrix[inBoneIndex.z] * inBoneWeight.z;
	boneTransform += boneMatrix[inBoneIndex.w] * inBoneWeight.w;

	mat3 boneTransformIT = boneMatrixIT[inBoneIndex.x] * inBoneWeight.x;
	boneTransformIT += boneMatrixIT[inBoneIndex.y] * inBoneWeight.y;
	boneTransformIT += boneMatrixIT[inBoneIndex.z] * inBoneWeight.z;
	boneTransformIT += boneMatrixIT[inBoneIndex.w] * inBoneWeight.w;

	gl_Position = viewProjectionMatrix * worldMatrix * boneTransform * vec4(inPosition, 1.0);

	vs_fs_normal = mat3(worldMatrix) * boneTransformIT * inNormal;

	vs_fs_color = inColor;
}