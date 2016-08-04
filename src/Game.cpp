#include <cassert>

#include "Error.hpp"
#include "Game.hpp"

Game::Game()
{
	if (!load())
		return;

	// update every 8ms (125 times a second)
	const unsigned int UPDATE_STEP = 8;
	unsigned int totalTime = 0;
	unsigned int oldTime = SDL_GetTicks();

	assert(window);
	while (!window->getDone())
	{
		const unsigned int newTime = SDL_GetTicks();
		unsigned int deltaTime = newTime - oldTime;
		totalTime += deltaTime;

		while (totalTime >= UPDATE_STEP)
		{
			update((float)deltaTime);
			totalTime -= UPDATE_STEP;
		}

		render();

		oldTime = newTime;
	}
}

Game::~Game()
{
	if (sceneRenderer) delete sceneRenderer;
	if (lightManager) delete lightManager;
	if (modelManager) delete modelManager;
	if (camera) delete camera;
	if (input) delete input;
	if (window) delete window;
}

bool Game::load()
{
	if (!Error::checkMemory(window = new Window())) return false;
	if (!window->load(1280, 720)) return false;

	if (!Error::checkMemory(input = new Input())) return false;

	assert(window);
	if (!Error::checkMemory(camera = new Camera(glm::vec3(0.0f, 100.0f, 0.0f), window))) return false;

	if (!Error::checkMemory(modelManager = new ModelManager())) return false;
	if (!modelManager->load()) return false;
	if (!modelManager->loadModel("Models\\Sponza.obj")) return false;
	if (!modelManager->loadModel("Models\\ArmyPilot.x", glm::vec3(-200.0f - 80.0f, 0.0f, 525.0f))) return false;
	if (!modelManager->loadModel("Models\\ArmyPilot.x", glm::vec3(200.0f - 80.0f, 0.0f, 525.0f))) return false;

	if (!Error::checkMemory(lightManager = new LightManager())) return false;
	if (!lightManager->addDirectionalLight(glm::vec3(0.2f, 1.0f, 0.2f), glm::vec3(1.0f, 0.5f, 0.0f))) return false;
	if (!lightManager->addDirectionalLight(glm::vec3(-0.9f, 1.0f, -0.8f), glm::vec3(0.0f, 0.5f, 1.0f))) return false;

	assert(modelManager);
	assert(lightManager);
	if (!Error::checkMemory(sceneRenderer = new SceneRenderer())) return false;
	if (!sceneRenderer->load(modelManager, lightManager)) return false;

	//weaponModel = new AnimatedModel();
	//if (!weaponModel->load("Models\\aktest.3ds"))
		//return false;

	//soldierModel = new AnimatedModel();
	//if (!soldierModel->load("Models\\ArmyPilot.x"))
		//return false;

	return true;
}

void Game::update(float deltaTime)
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
	camera->update(input, deltaTime);

	assert(modelManager);
	for (unsigned int i = 0; i < modelManager->getNumModels(); ++i)
	{
		Model *model = modelManager->getModelAt(i);
		assert(model);
		model->update(deltaTime);
	}

	//assert(weaponModel);
	//weaponModel->update(deltaTime);

	//assert(soldierModel);
	//soldierModel->update(deltaTime);
}

void Game::render()
{
	assert(sceneRenderer);
	assert(camera);
	sceneRenderer->render(camera);

	//assert(skinnedForwardShader);
	//glUseProgram(skinnedForwardShader->getProgram());

	//assert(weaponModel);
	//weaponModel->setPosition(glm::vec3(-100.0f, 100.0f, -150.0f));//(camera->getPosition() - camera->getRight() * 6.5f + camera->getForward() * 10.0f - camera->getUp() * 5.5f);
	//weaponModel->setPitchYawRoll(camera->getPitch(), camera->getYaw(), camera->getRoll());
	//weaponModel->setScale(6.0f);
	//skinnedForwardShader->setWorldViewProjectionUniforms(weaponModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	//weaponModel->render(skinnedForwardShader);

	//assert(soldierModel);
	//skinnedForwardShader->setWorldViewProjectionUniforms(soldierModel->getWorldMatrix(), camera->viewMatrix, camera->projectionMatrix);
	//soldierModel->render(skinnedForwardShader);
	
	window->finalizeFrame();
}