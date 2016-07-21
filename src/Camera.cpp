#include <gtc\matrix_transform.hpp>

#include "Camera.hpp"

const float Camera::MOVEMENT_SPEED = 0.5f;
const float Camera::PITCH_LOCK = 90.0f;

float Camera::mouseSensitivity = 30.f;

Camera::Camera(const glm::vec3 &position, Window *window) : Transform(position)
{
	this->window = window;

	nearClipPlane = 0.1f;
	farClipPlane = 5000.0f;

	viewMatrix = glm::mat4(1.0f);
	setFOV(90.f);
}

void Camera::rotatePitch(float amount)
{
	pitch += amount * mouseSensitivity;

	if (pitch < -PITCH_LOCK)
		pitch = -PITCH_LOCK;
	else if (pitch > PITCH_LOCK)
		pitch = PITCH_LOCK;
}

void Camera::rotateYaw(float amount)
{
	yaw -= amount * mouseSensitivity;
}

void Camera::rotateRoll(float amount)
{
	roll += amount * mouseSensitivity;
}

void Camera::doKeyboardInput(const Input *input, float delta)
{
	assert(input);

	if (input->isForwardKeyDown() && !input->isBackwardKeyDown())
		position += getForward() * MOVEMENT_SPEED * delta;
	else if (input->isBackwardKeyDown() && !input->isForwardKeyDown())
		position -= getForward() * MOVEMENT_SPEED * delta;

	if (input->isLeftKeyDown() && !input->isRightKeyDown())
		position += getRight() * MOVEMENT_SPEED * delta;
	else if (input->isRightKeyDown() && !input->isLeftKeyDown())
		position -= getRight() * MOVEMENT_SPEED * delta;
}

void Camera::doMouseLook(const Input *input)
{
	assert(window);
	const int halfScreenWidth = window->getWidth() / 2;
	const int halfScreenHeight = window->getHeight() / 2;

	assert(input);
	rotateYaw((input->getMouseX() - halfScreenWidth) / (float)halfScreenWidth);
	rotatePitch((input->getMouseY() - halfScreenHeight) / (float)halfScreenHeight);

	window->warpMouse(halfScreenWidth, halfScreenHeight);
}

void Camera::update(const Input *input, float delta)
{
	assert(input);
	doMouseLook(input);
	doKeyboardInput(input, delta);

	Transform::update(delta);

	viewMatrix = glm::lookAt(position, position + getForward(), getUp());
}

void Camera::setFOV(float fov)
{
	this->fov = fov;

	assert(window);
	projectionMatrix = glm::perspectiveFov(glm::radians(fov), (float)window->getWidth(), (float)window->getHeight(), nearClipPlane, farClipPlane);
}