#pragma once

#include <unordered_map>

#include "Shader.hpp"

class ShaderProgram
{
private:
	Shader *vertexShader, *fragmentShader;
	GLuint handle;
	std::unordered_map<std::string, GLint> uniforms;

public:
	~ShaderProgram();

	bool load(Shader *vertexShader, Shader *fragmentShader);
	GLint getUniform(const std::string &name);

	inline GLuint getHandle() const { return handle; }
};