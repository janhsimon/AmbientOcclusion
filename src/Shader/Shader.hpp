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
	bool load(const std::string &filename, GLenum type);
	void unload();

	inline GLuint getHandle() const { return handle; }
	inline std::string getFilename() const { return filename; }
};