#include <assert.h>

#include "GBuffer.hpp"
#include "..\Error.hpp"

GBuffer::~GBuffer()
{
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(3, textures);
	glDeleteTextures(1, &depthTexture);

	Error::checkGL();
}

bool GBuffer::load(unsigned int screenWidth, unsigned int screenHeight)
{
	// create the FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// create the geometry buffer textures
	glGenTextures(3, textures);
	glGenTextures(1, &depthTexture);

	for (unsigned int i = 0; i < 3; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// depth
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, screenWidth, screenHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// set up our multiple rendertargets
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, drawBuffers);

	if (!Error::checkGLFramebuffer()) return false;

	// restore default FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (!Error::checkGL()) return false;

	return true;
}

GLuint GBuffer::getFBO()
{
	return FBO;
}

GLuint GBuffer::getTexture(unsigned int index)
{
	assert(index < 3);
	return textures[index];
}
