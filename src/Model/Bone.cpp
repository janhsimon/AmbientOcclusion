#include "Bone.hpp"
#include "MathHelper.hpp"

Bone::Bone(const std::string &name, glm::mat4 &inverseBindPoseMatrix)
{
	this->name = name;
	this->inverseBindPoseMatrix = inverseBindPoseMatrix;
}

glm::mat4 Bone::getFinalMatrix() const
{
	// TODO: boneMatrix used to be an aiMatrix4x4 which is transposed, check if this code needs to be flipped!

	glm::mat4 boneMatrix = flatMatrix;

	Bone *currentBone = parent;
	while (currentBone)
	{
		boneMatrix = currentBone->flatMatrix * boneMatrix;
		currentBone = currentBone->parent;
	}
	
	return boneMatrix * inverseBindPoseMatrix;
}