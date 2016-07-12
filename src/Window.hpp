#pragma once

#include <SDL.h>

class Window
{
private:
	SDL_Window *window;
	SDL_GLContext context; // this is a (void) pointer
	bool done;

	bool initSDL();
	bool initGL();

public:
	~Window();

	bool load();
	void finalizeFrame();

	//inline SDL_Window *getSDLWindow() { return window; }

	inline bool getDone() { return done; }
	inline void setDone(bool done) { this->done = done; }
};