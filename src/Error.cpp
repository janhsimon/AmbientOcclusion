#include <glew.h>
#include <SDL.h>

#include "Error.hpp"

std::string Error::getOpenGLErrorString(GLenum errorFlag)
{
	if (errorFlag == GL_NO_ERROR)
		return "No error has been recorded.";
	else if (errorFlag == GL_INVALID_ENUM)
		return "An unacceptable value is specified for an enumerated argument.";
	else if (errorFlag == GL_INVALID_VALUE)
		return "A numeric argument is out of range.";
	else if (errorFlag == GL_INVALID_OPERATION)
		return "The specified operation is not allowed in the current state.";
	else if (errorFlag == GL_INVALID_FRAMEBUFFER_OPERATION)
		return "The framebuffer object is not complete.";
	else if (errorFlag == GL_OUT_OF_MEMORY)
		return "There is not enough memory left to execute the command.";
	else if (errorFlag == GL_STACK_UNDERFLOW)
		return "An attempt has been made to perform an operation that would cause an internal stack to underflow.";
	else if (errorFlag == GL_STACK_OVERFLOW)
		return "An attempt has been made to perform an operation that would cause an internal stack to overflow.";
	else
		return "Unrecognized error";
}

std::string Error::getOpenGLFrameBufferErrorString(GLenum errorFlag)
{
	if (errorFlag == GL_FRAMEBUFFER_COMPLETE)
		return "No error has been recorded.";
	else if (errorFlag == GL_FRAMEBUFFER_UNDEFINED)
		return "Target is the default framebuffer, but the default framebuffer does not exist.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
		return "The framebuffer attachment points are framebuffer incomplete.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
		return "The framebuffer does not have at least one image attached to it.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
		return "The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for a color attachment point named by GL_DRAW_BUFFERi.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
		return "GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER.";
	else if (errorFlag == GL_FRAMEBUFFER_UNSUPPORTED)
		return "The combination of internal formats of the attached images violates an implementation-dependent set of restrictions.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
		return "The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers.";
	else if (errorFlag == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
		return "A framebuffer attachment is layered and any populated attachment is not layered or all populated color attachments are not from textures of the same target.";
	else
		return "Unrecognized error";
}

void Error::report(const std::string &text)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", text.c_str(), nullptr);
}

bool Error::checkMemory(void *pointer)
{
	if (!pointer)
	{
		report("Failed to allocate memory.");
		return false;
	}

	return true;
}

bool Error::checkGL()
{
	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
	{
		report("OpenGL error: " + getOpenGLErrorString(error));
		return false;
	}

	return true;
}

bool Error::checkGLFramebuffer()
{
	GLenum error = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (error != GL_FRAMEBUFFER_COMPLETE)
	{
		report("OpenGL error: " + getOpenGLFrameBufferErrorString(error));
		return false;
	}

	return true;
}