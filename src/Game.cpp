#include <cassert>
#include <glew.h>

#include "Game.hpp"

Game::Game()
{
	window = new Window();
	assert(window);

	if (!window->load())
		return;

	while (!window->getDone())
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				window->setDone(true);
		}

		render();
	}
}

Game::~Game()
{
	if (window)
		delete window;
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	window->finalizeFrame();
}