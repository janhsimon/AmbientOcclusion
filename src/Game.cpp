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
	if (sponzaModel)
		delete sponzaModel;

	if (pistolModel)
		delete pistolModel;

	if (soldierModel)
		delete soldierModel;

	if (simpleForwardShader)
		delete simpleForwardShader;

	if (skinnedForwardShader)
		delete skinnedForwardShader;

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

	sponzaModel = new Model();
	if (!sponzaModel->load("Models\\Sponza.obj"))
		return false;

	pistolModel = new Model();
	if (!pistolModel->load("Models\\Pistol.obj"))
		return false;

	soldierModel = new AnimatedModel();
	if (!soldierModel->load("Models\\ArmyPilot.x"))
		return false;

	simpleForwardShader = new SimpleForwardShader();
	if (!simpleForwardShader->load())
		return false;

	skinnedForwardShader = new SkinnedForwardShader();
	if (!skinnedForwardShader->load())
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
			if (event.key.keysym.sym == SDLK_F1)
			{
				assert(soldierModel);
				soldierModel->changeAnimation();
			}
			else
			{
				assert(input);
				input->sendKeyboardKeyUpEvent(event);
			}
		}
		else if (event.type == SDL_QUIT)
			window->setDone(true);
	}

	assert(camera);
	assert(input);
	camera->update(input, delta);

	assert(sponzaModel);
	sponzaModel->update(delta);

	assert(pistolModel);
	pistolModel->update(delta);

	assert(soldierModel);
	soldierModel->update(delta);
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(simpleForwardShader);
	assert(sponzaModel);
	glUseProgram(simpleForwardShader->getProgram());
	simpleForwardShader->setWorldViewProjectionUniforms(sponzaModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	sponzaModel->render();

	assert(pistolModel);
	pistolModel->setPosition(camera->getPosition() - camera->getRight() * 6.5f + camera->getForward() * 10.0f - camera->getUp() * 5.5f);
	pistolModel->setPitchYawRoll(camera->getPitch(), camera->getYaw(), camera->getRoll());
	pistolModel->setScale(6.0f);
	simpleForwardShader->setWorldViewProjectionUniforms(pistolModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	pistolModel->render();

	assert(skinnedForwardShader);
	assert(soldierModel);
	glUseProgram(skinnedForwardShader->getProgram());
	skinnedForwardShader->setWorldViewProjectionUniforms(soldierModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	soldierModel->render(skinnedForwardShader);
	
	window->finalizeFrame();
}