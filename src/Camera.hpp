#pragma once

#include "Input.hpp"
#include "Transform.hpp"
#include "Window.hpp"

class Camera : public Transform
{
private:
	static const float MOVEMENT_SPEED;
	static const float PITCH_LOCK;

	Window *window;
	float fov;

	void doKeyboardInput(const Input *input, float delta);
	void doMouseLook(const Input *input);

public:
	glm::mat4 viewMatrix, projectionMatrix;
	float nearClipPlane, farClipPlane;
	static float mouseSensitivity;

	Camera(const glm::vec3 &position, Window *window);

	void rotatePitch(float amount);
	void rotateYaw(float amount);
	void rotateRoll(float amount);
	
	void update(const Input *input, float delta);

	inline float getFOV() const { return fov; }
	void setFOV(float fov);
};