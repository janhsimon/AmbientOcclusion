#include <glew.h>
#include <SDL.h>

int main(int argc, char **argv)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		//Error::report("Error", "SDL_Init Error: " + std::string(SDL_GetError()));
		return -1;
	}

	// use opengl version 3.1 (from 4.4)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// default buffer sizes
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Window *window = SDL_CreateWindow("Ambient Occlusion -- written by Jan Simon", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);

	if (!window)
	{
		//Error::report("Error", "SDL_CreateWindow Error" + std::string(SDL_GetError()));
		return -1;
	}

	// create an opengl context for the window
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	if (glContext == NULL)
	{
		//Error::report("Error", "SDL_GL_CreateContext Error: " + std::string(SDL_GetError()));
		return -1;
	}

	if (SDL_GL_SetSwapInterval(0) != 0)
	// disable vsync
	{
		//Error::report("Error", "SDL_GL_SetSwapInterval Error: " + std::string(SDL_GetError()));
		return -1;
	}

	if (glewInit() != GLEW_OK)
	{
		//Error::report("Error", "Failed to initialize GLEW.");
		return -1;
	}

	if (!GLEW_VERSION_3_1)
	{
		//Error::report("Error", "OpenGL v3.1 is required but your system does not support it.");
		return -1;
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		//Error::report("Error", "Failed to initialize OpenGL: " + Error::getOpenGLErrorString(error));
		return -1;
	}

	bool done = false;
	while (!done)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = true;
		}

		// set clear color
		glClearColor(0.2f, 0.5f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//finalize
		SDL_GL_SwapWindow(window);
	}

	return 0;
}