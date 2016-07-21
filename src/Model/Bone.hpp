#pragma once

#include <assimp\scene.h>
#include <glm.hpp>
#include <vector>

class Bone
{
private:
	glm::mat4 inverseBindPoseMatrix;
	const aiNode *rootNode;
	const aiNode *node;

public:
	Bone(glm::mat4 &inverseBindPoseMatrix, const aiNode *rootNode, const aiNode *node);

	glm::mat4 getFinalMatrix();
};