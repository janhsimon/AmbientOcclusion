#include <cassert>
#include <glew.h>

#include "Error.hpp"
#include "Window.hpp"

Window::~Window()
{
	if (context)
		SDL_GL_DeleteContext(context);

	if (window)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

bool Window::initSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		return false;

	// use opengl version 3.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window = SDL_CreateWindow("Ambient Occlusion -- written by Jan Simon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

	if (!window)
		return false;

	SDL_ShowCursor(0);

	return true;
}

bool Window::initGL()
{
	assert(window);

	// create an opengl context for the window
	context = SDL_GL_CreateContext(window);

	if (!context)
		return false;

	if (SDL_GL_SetSwapInterval(0) != 0)
	// disable vsync
		return false;

	if (glewInit() != GLEW_OK)
		return false;

	if (!GLEW_VERSION_3_1)
		return false;

	if (glGetError() != GL_NO_ERROR)
		return false;

	glClearColor(0.3f, 0.3f, 0.6f, 1.0f);

	return true;
}

bool Window::load(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	if (!initSDL())
	{
		Error::report("Failed to create SDL window.");
		return false;
	}

	if (!initGL())
	{
		Error::report("Failed to create OpenGL context.");
		return false;
	}

	done = false;
	return true;
}

void Window::warpMouse(unsigned int x, unsigned int y)
{
	SDL_WarpMouseInWindow(window, x, y);
}

void Window::finalizeFrame()
{
	SDL_GL_SwapWindow(window);
}