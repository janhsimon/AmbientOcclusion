#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

#include "ShaderProgram.hpp"

class ForwardShader : public ShaderProgram
{
private:
	static const std::string WORLD_MATRIX_UNIFORM_NAME;
	static const std::string VIEW_PROJECTION_MATRIX_UNIFORM_NAME;
	static const std::string BONE_MATRIX_UNIFORM_NAME;
	static const std::string BONE_MATRIX_IT_UNIFORM_NAME;

	GLint worldMatrixUniformLocation, viewProjectionMatrixUniformLocation, boneMatrixUniformLocation, boneMatrixITUniformLocation;

public:
	bool create();

	void setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewProjectionMatrix);
	void setBoneMatrixUniforms(float *boneMatrices);
	void setBoneMatrixITUniforms(float *boneMatricesIT);
};