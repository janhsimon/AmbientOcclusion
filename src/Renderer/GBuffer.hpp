#pragma once

#include <glew.h>

class GBuffer
{
private:
	GLuint FBO;
	GLuint textures[2];
	GLuint depthTexture;

public:
	~GBuffer();

	bool load(unsigned int screenWidth, unsigned int screenHeight);

	GLuint getFBO();
	GLuint getTexture(unsigned int index);
	GLuint getDepthTexture();
};