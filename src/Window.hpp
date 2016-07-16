#pragma once

#include <SDL.h>
#include <string>

class Window
{
private:
	SDL_Window *window;
	SDL_GLContext context; // this is a (void) pointer
	unsigned int width, height;
	bool done;

	bool initSDL();
	bool initGL();

public:
	~Window();

	bool load(unsigned int width, unsigned int height);
	void warpMouse(unsigned int x, unsigned int y);
	void setTitleInfo(const std::string &info);
	void finalizeFrame();

	inline unsigned int getWidth() const { return width; }
	inline unsigned int getHeight() const { return height; }
	inline bool getDone() const { return done; }
	inline void setDone(bool done) { this->done = done; }
};