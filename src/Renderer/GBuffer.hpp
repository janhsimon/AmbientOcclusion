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

	GLuint getFBO();
	GLuint getTexture(unsigned int index);
};