#include <sstream>
#include <gtc\type_ptr.hpp>

#include "SimpleForwardShader.hpp"
#include "..\Error.hpp"

const std::string SimpleForwardShader::WORLD_MATRIX_UNIFORM_NAME = "worldMatrix";
const std::string SimpleForwardShader::VIEW_PROJECTION_MATRIX_UNIFORM_NAME = "viewProjectionMatrix";

bool SimpleForwardShader::load()
{
	if (!ShaderProgram::load("Shaders\\Simple.vs.glsl", "", "Shaders\\Forward.fs.glsl"))
		return false;

	if (!link())
		return false;

	glUseProgram(program);

	if (!getUniformLocation(WORLD_MATRIX_UNIFORM_NAME, worldMatrixUniformLocation))
		return false;

	if (!getUniformLocation(VIEW_PROJECTION_MATRIX_UNIFORM_NAME, viewProjectionMatrixUniformLocation))
		return false;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		Error::report("Failed to load forward shader, error code: " + error);
		return false;
	}

	return true;
}

void SimpleForwardShader::setWorldViewProjectionUniforms(const glm::mat4 &worldMatrix, const glm::mat4 &viewMatrix, const glm::mat4 &projectionMatrix)
{
	glUniformMatrix4fv(worldMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	glUniformMatrix4fv(viewProjectionMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix));
}