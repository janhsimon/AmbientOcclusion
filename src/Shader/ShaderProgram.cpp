#include <cassert>
#include <fstream>
#include <sstream>

#include "ShaderProgram.hpp"
#include "..\Error.hpp"

bool ShaderProgram::loadShader(GLuint shader, const std::string &filename)
{
	assert(shader != 0);

	std::ifstream file(filename);

	if (!file)
	{
		Error::report("Failed to open shader file \"" + filename + "\".");
		return false;
	}

	std::stringstream source;
	source << file.rdbuf();

	const std::string &tmp = source.str();
	const char *c_str = tmp.c_str();

	glShaderSource(shader, 1, &c_str, nullptr);
	
	if (!compileShader(shader, filename))
		return false;

	glAttachShader(program, shader);

	return true;
}

bool ShaderProgram::compileShader(GLuint shader, const std::string &filename)
{
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

		GLchar *log = new GLchar[len + 1];
		
		glGetShaderInfoLog(shader, len, &len, log);

		std::stringstream s;
		s << "Failed to compile shader \"" << filename << "\":" << std::endl << std::endl << log;
		Error::report(s.str());

		delete[] log;

		return false;
	}

	return true;
}

bool ShaderProgram::load(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename, const std::string &fragmentShaderFilename)
{
	program = 0;
	vertexShader = 0;
	geometryShader = 0;
	fragmentShader = 0;

	program = glCreateProgram();

	if (!program)
	{
		Error::report("Failed to create shader program.");
		return false;
	}
	
	if (!vertexShaderFilename.empty())
	{
		vertexShader = glCreateShader(GL_VERTEX_SHADER);

		if (!loadShader(vertexShader, vertexShaderFilename))
			return false;
	}

	if (!geometryShaderFilename.empty())
	{
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

		if (!loadShader(geometryShader, geometryShaderFilename))
			return false;
	}

	if (!fragmentShaderFilename.empty())
	{
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		if (!loadShader(fragmentShader, fragmentShaderFilename))
			return false;
	}

	this->filename = fragmentShaderFilename;

	return true;
}

bool ShaderProgram::link()
{
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar *log = new GLchar[len + 1];

		glGetProgramInfoLog(program, len, &len, log);

		std::stringstream s;
		s << "Failed to link shader program \"" << filename << "\":" << std::endl << std::endl << log;
		Error::report(s.str());

		delete[] log;

		return false;
	}

	return true;
}

const bool ShaderProgram::getUniformLocation(const std::string &uniformName, GLint &locationOut)
{
	locationOut = glGetUniformLocation(program, uniformName.c_str());

	if (locationOut < 0)
	{
		Error::report("Failed to get location of uniform \"" + uniformName + "\" in shader \"" + filename + "\".");
		return false;
	}

	return true;
}

ShaderProgram::~ShaderProgram()
{
	glUseProgram(0);

	if (vertexShader > 0)
		glDeleteShader(vertexShader);

	if (geometryShader > 0)
		glDeleteShader(geometryShader);

	if (fragmentShader > 0)
		glDeleteShader(fragmentShader);

	if (program > 0)
		glDeleteProgram(program);
}
