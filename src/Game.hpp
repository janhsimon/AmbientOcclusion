#pragma once

#include "Camera.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "Model\ModelManager.hpp"
#include "Renderer\SceneRenderer.hpp"

class Game
{
private:
	Window *window;
	Input *input;
	Camera *camera;
	ModelManager *modelManager;
	SceneRenderer *sceneRenderer;
	
	bool load();
	void update(float deltaTime);
	void render();
		
public:
	Game();
	~Game();
};