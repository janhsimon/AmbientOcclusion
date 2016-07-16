#include <cassert>

#include "Input.hpp"

Input::Input()
{
	forwardKeyDown = backwardKeyDown = leftKeyDown = rightKeyDown = false;
	mouseX = mouseY = 0;

	forwardKey = SDLK_w;
	backwardKey = SDLK_s;
	leftKey = SDLK_a;
	rightKey = SDLK_d;
}

void Input::sendMouseMoveEvent(const SDL_Event &event)
{
	assert(event.type == SDL_MOUSEMOTION);
	mouseX = event.motion.x;
	mouseY = event.motion.y;
}

void Input::sendKeyboardKeyDownEvent(const SDL_Event &event)
{
	assert(event.type == SDL_KEYDOWN);

	if (event.key.keysym.sym == forwardKey)
		forwardKeyDown = true;
	else if (event.key.keysym.sym == backwardKey)
		backwardKeyDown = true;
	else if(event.key.keysym.sym == leftKey)
		leftKeyDown = true;
	else if(event.key.keysym.sym == rightKey)
		rightKeyDown = true;
}

void Input::sendKeyboardKeyUpEvent(const SDL_Event &event)
{
	assert(event.type == SDL_KEYUP);

	if (event.key.keysym.sym == forwardKey)
		forwardKeyDown = false;
	else if (event.key.keysym.sym == backwardKey)
		backwardKeyDown = false;
	else if (event.key.keysym.sym == leftKey)
		leftKeyDown = false;
	else if (event.key.keysym.sym == rightKey)
		rightKeyDown = false;
}