#include <fstream>
#include <sstream>

#include "Shader.hpp"
#include "..\Error.hpp"

Shader::~Shader()
{
	glDeleteShader(handle);

	Error::checkGL();
}

bool Shader::load(const std::string &filename, GLenum type)
{
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

	handle = glCreateShader(type);
	glShaderSource(handle, 1, &c_str, nullptr);
	glCompileShader(handle);

	GLint compiled;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLsizei len;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &len);

		GLchar *log;
		if (!Error::checkMemory(log = new GLchar[len + 1])) return false;
		glGetShaderInfoLog(handle, len, &len, log);

		std::stringstream s;
		s << "Failed to compile shader \"" << filename << "\":" << std::endl << std::endl << log;
		Error::report(s.str());

		delete[] log;
		return false;
	}

	if (!Error::checkGL())
		return false;
	
	this->filename = filename;

	return true;
}