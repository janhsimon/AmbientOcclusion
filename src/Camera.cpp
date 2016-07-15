#include <gtc\matrix_transform.hpp>

#include "Camera.hpp"

const float Camera::MOVEMENT_SPEED = 5.0f;
const float Camera::PITCH_LOCK = 90.0f;

float Camera::mouseSensitivity = 30.f;

Camera::Camera(const glm::vec3 &position, unsigned int screenWidth, unsigned int screenHeight) : Transform(position)
{
	nearClipPlane = 1.0f;
	farClipPlane = 5000.0f;

	viewMatrix = glm::mat4(1.0f);
	setFOV(90.f, screenWidth, screenHeight);
}

void *Camera::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void Camera::operator delete(void *p)
{
	_aligned_free(p);
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

void Camera::update(float delta)
{
	//acceleration = glm::vec3(0.f, 0.f, 0.f);

	/*
	if (inputManager->forwardKeyPressed && !inputManager->backKeyPressed)
		position += getForward() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);
	else if (inputManager->backKeyPressed && !inputManager->forwardKeyPressed)
		position -= getForward() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);

	if (inputManager->leftKeyPressed && !inputManager->rightKeyPressed)
		position += getRight() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);
	else if (inputManager->rightKeyPressed && !inputManager->leftKeyPressed)
		position -= getRight() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);

	if (inputManager->upKeyPressed && !inputManager->downKeyPressed)
		position += getUp() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);
	else if (inputManager->downKeyPressed && !inputManager->upKeyPressed)
		position -= getUp() * MOVEMENT_SPEED * delta * (inputManager->crouchKeyPressed ? CROUCH_SPEED_FACTOR : 1.f);
	*/

	updateTransform(delta);

	viewMatrix = glm::lookAt(position, position + getForward(), getUp());
}

void Camera::setFOV(float fov, unsigned int screenWidth, unsigned int screenHeight)
{
	this->fov = fov;

	projectionMatrix = glm::perspectiveFov(glm::radians(fov), (float)screenWidth, (float)screenHeight, nearClipPlane, farClipPlane);
}