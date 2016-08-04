#pragma once

#include <glew.h>
#include <glm.hpp>
#include <string>

class Shader
{
private:
	GLuint handle;
	std::string filename;

public:
	~Shader();

	bool load(const std::string &filename, GLenum type);

	inline GLuint getHandle() const { return handle; }
	inline std::string getFilename() const { return filename; }
};