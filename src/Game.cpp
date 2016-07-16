#include <cassert>
#include <glew.h>
#include <sstream>

#include "Error.hpp"
#include "Game.hpp"

Game::Game()
{
	if (!load())
		return;

	unsigned int thisTickTime, lastTickTime = 0;
	unsigned int ms = 0;
	float frameSampleSpacer = 0.0f;

	assert(window);
	while (!window->getDone())
	{
		thisTickTime = SDL_GetTicks();
		float delta = (thisTickTime - lastTickTime) * 1.0f;

		frameSampleSpacer -= delta;

		if (frameSampleSpacer <= 0.0f)
		{
			ms = (thisTickTime - lastTickTime);
			frameSampleSpacer = 1000.0f;
		}

		int fps = 0;

		if (ms > 0.0f)
			fps = int(1000.0f / ms);

		std::stringstream s;
		s << fps << "fps " << ms << "ms";
		window->setTitleInfo(s.str());

		update(delta);
		render();

		lastTickTime = thisTickTime;
	}
}

Game::~Game()
{
	if (testModel)
		delete testModel;

	if (camera)
		delete camera;

	if (input)
		delete input;

	if (window)
		delete window;
}

bool Game::load()
{
	window = new Window();
	if (!window->load(1280, 720))
		return false;

	input = new Input();

	assert(window);
	camera = new Camera(glm::vec3(0.0f), window);

	testModel = new Model(glm::vec3(0.0f, 0.0f, 1.5f));
	if (!testModel->load("Models\\Sponza.obj"))
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
		if (event.type == SDL_MOUSEMOTION)
		{
			assert(input);
			input->sendMouseMoveEvent(event);
		}
		else if (event.type == SDL_KEYDOWN)
		{
			assert(input);
			input->sendKeyboardKeyDownEvent(event);
		}
		else if (event.type == SDL_KEYUP)
		{
			assert(input);
			input->sendKeyboardKeyUpEvent(event);
		}
		else if (event.type == SDL_QUIT)
			window->setDone(true);
	}

	assert(camera);
	assert(input);
	camera->update(input, delta);

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