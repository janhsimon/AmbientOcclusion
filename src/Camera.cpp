#include <gtc\matrix_transform.hpp>

#include "Camera.hpp"

const float Camera::MOVEMENT_SPEED = 3.5f;

float Camera::mouseSensitivity = 1.0f;

Camera::Camera(const glm::vec3 &position, Window *window) : Transform(position)
{
	this->window = window;

	nearClipPlane = 0.1f;
	farClipPlane = 5000.0f;

	setFOV(90.f);

	throwAwayFirstMouseMove = true;
}

void Camera::doKeyboardInput(const Input *input, float deltaTime)
{
	assert(input);

	if (input->isForwardKeyDown() && !input->isBackwardKeyDown())
		position += getForward() * MOVEMENT_SPEED * deltaTime;
	else if (input->isBackwardKeyDown() && !input->isForwardKeyDown())
		position -= getForward() * MOVEMENT_SPEED * deltaTime;

	if (input->isLeftKeyDown() && !input->isRightKeyDown())
		position -= getRight() * MOVEMENT_SPEED * deltaTime;
	else if (input->isRightKeyDown() && !input->isLeftKeyDown())
		position += getRight() * MOVEMENT_SPEED * deltaTime;
}

void Camera::doMouseLook(const Input *input)
{
	assert(window);
	const unsigned int halfScreenWidth = window->getWidth() / 2;
	const unsigned int halfScreenHeight = window->getHeight() / 2;

	if (throwAwayFirstMouseMove)
		throwAwayFirstMouseMove = false;
	else
	{
		assert(input);
		float yaw	= ((input->getMouseX() - (float)halfScreenWidth) / halfScreenWidth) * mouseSensitivity;
		float pitch = ((input->getMouseY() - (float)halfScreenHeight) / halfScreenHeight) * mouseSensitivity;
		rotateYaw(yaw);
		rotatePitch(-pitch);
	}

	window->warpMouse(halfScreenWidth, halfScreenHeight);
}

void Camera::update(const Input *input, float deltaTime)
{
	assert(input);
	doMouseLook(input);
	doKeyboardInput(input, deltaTime);

	Transform::update(deltaTime);
	viewMatrix = glm::lookAt(position, position + getForward(), getUp());
}

glm::vec4 Camera::projectPointToScreenSpace(const glm::vec4 &point) const
{
	glm::vec4 proj = projectionMatrix * viewMatrix * point;

	for (unsigned int i = 0; i < 3; ++i)
		proj[i] /= proj[3];
	
	return proj;
}

glm::vec4 Camera::projectPointToScreenSpace(const glm::vec3 &point) const
{
	return projectPointToScreenSpace(glm::vec4(point, 1.0f));
}

void Camera::setFOV(float fov)
{
	this->fov = fov;

	assert(window);
	projectionMatrix = glm::perspectiveFov(glm::radians(fov), (float)window->getWidth(), (float)window->getHeight(), nearClipPlane, farClipPlane);
}