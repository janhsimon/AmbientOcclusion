#pragma once

#include "Camera.hpp"
#include "ForwardShader.hpp"
#include "Model.hpp"
#include "Window.hpp"

class Game
{
private:
	Window *window;
	Camera *camera;
	Model *testModel;
	ForwardShader *forwardShader;

	bool load();
	void update(float delta);
	void render();
		
public:
	Game();
	~Game();
};