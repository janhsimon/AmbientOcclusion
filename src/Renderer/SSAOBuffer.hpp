#pragma once

#include <glew.h>

class SSAOBuffer
{
private:
	GLuint FBOs[2];
	GLuint textures[2];
	GLuint noiseTexture;

public:
	~SSAOBuffer();

	bool load(unsigned int screenWidth, unsigned int screenHeight);

	inline GLuint getFBO(unsigned int index) const { assert(index < 2); return FBOs[index]; }
	inline GLuint getTexture(unsigned int index) const { assert(index < 2); return textures[index]; }
	inline GLuint getNoiseTexture() const { return noiseTexture; }
};