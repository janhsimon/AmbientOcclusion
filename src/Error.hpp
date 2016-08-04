#pragma once

#include <glew.h>
#include <string>

class Error
{
private:
	static std::string getOpenGLErrorString(GLenum errorFlag);
	static std::string getOpenGLFrameBufferErrorString(GLenum errorFlag);

public:
	static void report(const std::string &text);
	static bool Error::checkMemory(void *pointer);
	static bool checkGL();
	static bool checkGLFramebuffer();
};