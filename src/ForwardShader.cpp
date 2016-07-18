#include <sstream>
#include <gtc\type_ptr.hpp>

#include "Error.hpp"
#include "ForwardShader.hpp"

const std::string ForwardShader::WORLD_MATRIX_UNIFORM_NAME = "worldMatrix";
const std::string ForwardShader::VIEW_PROJECTION_MATRIX_UNIFORM_NAME = "viewProjectionMatrix";
const std::string ForwardShader::BONE_MATRIX_UNIFORM_NAME = "boneMatrix";
const std::string ForwardShader::BONE_MATRIX_IT_UNIFORM_NAME = "boneMatrixIT";

bool ForwardShader::create()
{
	if (!load("Shaders\\Skinned.vs.glsl", "", "Shaders\\Forward.fs.glsl"))
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

	if (!getUniformLocation(BONE_MATRIX_IT_UNIFORM_NAME, boneMatrixITUniformLocation))
		return false;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		Error::report("Failed to load forward shader, error code: " + error);
		return false;
	}

	return true;
}

void ForwardShader::setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewProjectionMatrix)
{
	glUniformMatrix4fv(worldMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(viewProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(viewProjectionMatrix));
}

void ForwardShader::setBoneMatrixUniforms(float *boneMatrices)
{
	glUniformMatrix4fv(boneMatrixUniformLocation, 64, GL_FALSE, boneMatrices);
}

void ForwardShader::setBoneMatrixITUniforms(float *boneMatricesIT)
{
	glUniformMatrix3fv(boneMatrixITUniformLocation, 64, GL_FALSE, boneMatricesIT);
}