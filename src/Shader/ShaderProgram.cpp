#include <cassert>
#include <sstream>

#include "ShaderProgram.hpp"
#include "../Error.hpp"

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(handle);
}

bool ShaderProgram::load(Shader *vertexShader, Shader *fragmentShader)
{
	assert(vertexShader);
	this->vertexShader = vertexShader;

	assert(fragmentShader);
	this->fragmentShader = fragmentShader;

	handle = glCreateProgram();

	glAttachShader(handle, vertexShader->getHandle());
	glAttachShader(handle, fragmentShader->getHandle());
	glLinkProgram(handle);

	GLint linked;
	glGetProgramiv(handle, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &len);

		GLchar *log = new GLchar[len + 1];
		glGetProgramInfoLog(handle, len, &len, log);

		std::stringstream s;
		s << "Failed to link shaders \"" << vertexShader->getFilename() << "\" and \"" << fragmentShader->getFilename() << "\":" << std::endl << std::endl << log;
		Error::report(s.str());

		delete[] log;
		return false;
	}

	if (glGetError() != GL_NO_ERROR)
	{
		std::stringstream s;
		s << "Failed to link shaders \"" << vertexShader->getFilename() << "\" and \"" << fragmentShader->getFilename() << "\".";
		Error::report(s.str());
		return false;
	}

	return true;
}

bool ShaderProgram::registerUniform(const std::string &name)
{
	GLint location = glGetUniformLocation(handle, name.c_str());
	
	if (location < 0 || glGetError() != GL_NO_ERROR)
	{
		assert(vertexShader);
		assert(fragmentShader);
		std::stringstream s;
		s << "Failed to register uniform \"" << name << "\" for shaders \"" << vertexShader->getFilename() << "\" and \"" << fragmentShader->getFilename() << "\".";
		Error::report(s.str());
		return false;
	}

	uniforms[name] = location;

	return true;
}

GLint ShaderProgram::getUniform(const std::string &name)
{
	std::unordered_map<std::string, GLint>::const_iterator uniform = uniforms.find(name);
	assert(uniform != uniforms.end());
	return uniform->second;
}