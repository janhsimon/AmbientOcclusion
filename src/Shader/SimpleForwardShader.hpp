#pragma once

#include <glm.hpp>
#include <string>
#include <vector>

#include "ShaderProgram.hpp"

class SimpleForwardShader : public ShaderProgram
{
private:
	static const std::string WORLD_MATRIX_UNIFORM_NAME;
	static const std::string VIEW_PROJECTION_MATRIX_UNIFORM_NAME;

	GLint worldMatrixUniformLocation;
	GLint viewProjectionMatrixUniformLocation;

public:
	bool load();

	void setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix);
};