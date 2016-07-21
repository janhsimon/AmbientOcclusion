#pragma once

#include <glm.hpp>
#include <assimp\scene.h>

class MathHelper
{
public:
	static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
	static aiMatrix4x4 glmMat4ToAi(const glm::mat4 &from);
};