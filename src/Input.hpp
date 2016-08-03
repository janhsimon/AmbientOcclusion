#pragma once

#include <SDL.h>

class Input
{
private:
	bool forwardKeyDown, backwardKeyDown, leftKeyDown, rightKeyDown;
	unsigned int mouseX, mouseY;

	SDL_Keycode forwardKey, backwardKey, leftKey, rightKey;

public:
	Input();

	void sendMouseMoveEvent(const SDL_Event &event);
	void sendKeyboardKeyDownEvent(const SDL_Event &event);
	void sendKeyboardKeyUpEvent(const SDL_Event &event);

	inline bool isForwardKeyDown() const { return forwardKeyDown; }
	inline bool isBackwardKeyDown() const { return backwardKeyDown; }
	inline bool isLeftKeyDown() const { return leftKeyDown; }
	inline bool isRightKeyDown() const { return rightKeyDown; }

	inline unsigned int getMouseX() const { return mouseX; }
	inline unsigned int getMouseY() const { return mouseY; }
};