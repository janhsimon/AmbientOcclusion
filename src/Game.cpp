#include <cassert>
#include <glew.h>
#include <sstream>

#include "Error.hpp"
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

	testModel = new Model(glm::vec3(0.0f, 0.0f, 1.5f));
	if (!testModel->load("Models\\test.obj"))
		return false;

	forwardShader = new ForwardShader();
	if (!forwardShader->create())
		return false;

	return true;
}

void Game::update(float delta)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_MOUSEBUTTONUP)
		{
			if (event.button.button == 1)
			{
				assert(camera);
				std::stringstream s;
				s << "Camera yaw: " << camera->getYaw() << " pitch: " << camera->getPitch();
				Error::report(s.str());
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			assert(window);
			const int halfScreenWidth = window->getWidth() / 2;
			const int halfScreenHeight = window->getHeight() / 2;

			assert(camera);
			camera->rotateYaw((event.motion.x - halfScreenWidth) / (float)halfScreenWidth);
			camera->rotatePitch((event.motion.y - halfScreenHeight) / (float)halfScreenHeight);

			window->warpMouse(halfScreenWidth, halfScreenHeight);
		}
		else if (event.type == SDL_QUIT)
			window->setDone(true);
	}

	assert(camera);
	camera->update(delta);

	//assert(testModel);
	//testModel->update(delta);
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(forwardShader);
	glUseProgram(forwardShader->getProgram());
	assert(testModel);
	forwardShader->setWorldViewProjectionUniforms(testModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	testModel->render();

	window->finalizeFrame();
}