#include <cassert>
#include <glm.hpp>
#include <random>
#include <vector>

#include "SSAOBuffer.hpp"
#include "..\Error.hpp"

SSAOBuffer::~SSAOBuffer()
{
	glDeleteFramebuffers(2, FBOs);
	glDeleteTextures(2, textures);
	glDeleteTextures(1, &noiseTexture);

	Error::checkGL();
}

bool SSAOBuffer::load(unsigned int screenWidth, unsigned int screenHeight)
{
	glGenFramebuffers(2, FBOs);

	for (unsigned int i = 0; i < 2; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBOs[i]);

		glGenTextures(1, &textures[i]);
	
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);
	
		if (i == 0)
		{
			glGenTextures(1, &noiseTexture);
			std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f); // generates random floats between 0.0 and 1.0
			std::default_random_engine generator;
			std::vector<glm::vec3> ssaoNoise;

			for (GLuint i = 0; i < 16; i++)
			{
				glm::vec3 noise(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, 0.0f); // rotate around z-axis (in tangent space)
				ssaoNoise.push_back(noise);
			}

			glBindTexture(GL_TEXTURE_2D, noiseTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!Error::checkGL()) return false;
	return true;
}