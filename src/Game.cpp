#include <cassert>
#include <glew.h>

#include "Game.hpp"

Game::Game()
{
	if (!load())
		return;

	while (!window->getDone())
	{
		update(1.0f);
		render();
	}
}

Game::~Game()
{
	if (testModel)
		delete testModel;

	if (camera)
		delete camera;

	if (window)
		delete window;
}

bool Game::load()
{
	window = new Window();
	assert(window);

	if (!window->load(1280, 720))
		return false;

	camera = new Camera(glm::vec3(0.0f), window->getWidth(), window->getHeight());

	testModel = new Model(glm::vec3(0.0f));
	if (!testModel->load("test.obj"))
		return false;

	return true;
}

void Game::update(float delta)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			window->setDone(true);
	}

	assert(camera);
	camera->updateTransform(delta);

	assert(testModel);
	testModel->updateTransform(delta);
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	assert(testModel);
	testModel->render();

	window->finalizeFrame();
}