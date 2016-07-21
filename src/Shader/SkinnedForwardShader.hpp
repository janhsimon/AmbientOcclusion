#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

#include "ShaderProgram.hpp"

class SkinnedForwardShader : public ShaderProgram
{
private:
	static const std::string WORLD_MATRIX_UNIFORM_NAME;
	static const std::string VIEW_PROJECTION_MATRIX_UNIFORM_NAME;
	static const std::string BONE_MATRIX_UNIFORM_NAME;
	
	GLint worldMatrixUniformLocation, viewProjectionMatrixUniformLocation, boneMatrixUniformLocation;

public:
	bool load();

	void setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
	void setBoneMatrixUniforms(float *boneMatrices, unsigned int numBoneMatrices);
};