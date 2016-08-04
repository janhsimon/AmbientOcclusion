#pragma once

#include "Camera.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "Light\LightManager.hpp"
#include "Model\ModelManager.hpp"
#include "Renderer\SceneRenderer.hpp"

class Game
{
private:
	Window *window;
	Input *input;
	Camera *camera;
	ModelManager *modelManager;
	LightManager *lightManager;
	SceneRenderer *sceneRenderer;
	
	bool load();
	void update(float deltaTime);
	void render();
		
public:
	Game();
	~Game();
};