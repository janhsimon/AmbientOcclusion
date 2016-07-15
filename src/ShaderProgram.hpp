#pragma once

#include <glew.h>

class ShaderProgram
{
private:
	std::string filename;

	bool loadShader(GLuint shader, const std::string &filename);
	bool compileShader(GLuint shader, const std::string &filename);

protected:
	GLuint program;
	GLuint vertexShader, geometryShader, fragmentShader;

	~ShaderProgram();

	bool load(const std::string &vertexShaderFilename, const std::string &geometryShaderFilename, const std::string &fragmentShaderFilename);
	bool link();
	const bool getUniformLocation(const std::string &uniformName, GLint &locationOut);

public:
	inline GLuint getProgram() const { return program; }
};