#include "Bone.hpp"
#include "MathHelper.hpp"

Bone::Bone(glm::mat4 &inverseBindPoseMatrix, const aiNode *rootNode, const aiNode *node)
{
	assert(rootNode);
	assert(node);
	this->inverseBindPoseMatrix = inverseBindPoseMatrix;
	this->rootNode = rootNode;
	this->node = node;
}

glm::mat4 Bone::getFinalMatrix()
{
	const aiNode *currentNode = node;
	assert(currentNode);

	aiMatrix4x4 boneMatrix;
	do
	{
		boneMatrix = currentNode->mTransformation * boneMatrix;
		currentNode = currentNode->mParent;
	}
	while (currentNode);

	return MathHelper::aiMatrix4x4ToGlm(boneMatrix) * inverseBindPoseMatrix;
}