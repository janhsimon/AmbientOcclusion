#include <sstream>
#include <gtc\type_ptr.hpp>

#include "SkinnedForwardShader.hpp"
#include "..\Error.hpp"

const std::string SkinnedForwardShader::WORLD_MATRIX_UNIFORM_NAME = "worldMatrix";
const std::string SkinnedForwardShader::VIEW_PROJECTION_MATRIX_UNIFORM_NAME = "viewProjectionMatrix";
const std::string SkinnedForwardShader::BONE_MATRIX_UNIFORM_NAME = "boneMatrix";

bool SkinnedForwardShader::load()
{
	if (!ShaderProgram::load("Shaders\\Skinned.vs.glsl", "", "Shaders\\Forward.fs.glsl"))
		return false;

	if (!link())
		return false;

	glUseProgram(program);

	if (!getUniformLocation(WORLD_MATRIX_UNIFORM_NAME, worldMatrixUniformLocation))
		return false;

	if (!getUniformLocation(VIEW_PROJECTION_MATRIX_UNIFORM_NAME, viewProjectionMatrixUniformLocation))
		return false;

	if (!getUniformLocation(BONE_MATRIX_UNIFORM_NAME, boneMatrixUniformLocation))
		return false;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		Error::report("Failed to load forward shader, error code: " + error);
		return false;
	}

	return true;
}

void SkinnedForwardShader::setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
	glUniformMatrix4fv(worldMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(viewProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));
}

void SkinnedForwardShader::setBoneMatrixUniforms(float *boneMatrices, unsigned int numBoneMatrices)
{
	glUniformMatrix4fv(boneMatrixUniformLocation, numBoneMatrices, GL_FALSE, boneMatrices);
}