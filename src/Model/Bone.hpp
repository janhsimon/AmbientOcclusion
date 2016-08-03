#pragma once

#include <assimp\scene.h>
#include <glm.hpp>
#include <vector>

class Bone
{
private:
	Bone *parent;
	std::string name;
	glm::mat4 inverseBindPoseMatrix;
	glm::mat4 flatMatrix;

public:
	Bone(const std::string &name, glm::mat4 &inverseBindPoseMatrix);

	glm::mat4 getFinalMatrix() const;
	
	inline std::string getName() const { return name; }
	inline void setParent(Bone *bone) { this->parent = parent; }
	inline void setFlatMatrix(const glm::mat4 &flatMatrix) { this->flatMatrix = flatMatrix; }
};