#pragma once

#include <glew.h>

class GBuffer
{
private:
	GLuint FBO;
	GLuint textures[3];
	GLuint depthTexture;

public:
	~GBuffer();

	bool load(unsigned int screenWidth, unsigned int screenHeight);

	inline GLuint getFBO() const { return FBO; }
	inline GLuint getTexture(unsigned int index) const { assert(index < 3); return textures[index]; }
};