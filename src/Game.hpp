#pragma once

#include "Window.hpp"

class Game
{
private:
	Window *window;

	void render();
		
public:
	Game();
	~Game();
};