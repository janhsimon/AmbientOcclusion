#pragma once

#include "Camera.hpp"
#include "Input.hpp"
#include "Window.hpp"
#include "Model\AnimatedModel.hpp"
#include "Model\Model.hpp"
#include "Shader\SimpleForwardShader.hpp"
#include "Shader\SkinnedForwardShader.hpp"

class Game
{
private:
	Window *window;
	Input *input;
	Camera *camera;
	Model *sponzaModel;
	Model *pistolModel;
	AnimatedModel *soldierModel;
	SimpleForwardShader *simpleForwardShader;
	SkinnedForwardShader *skinnedForwardShader;

	bool load();
	void update(float delta);
	void render();
		
public:
	Game();
	~Game();
};