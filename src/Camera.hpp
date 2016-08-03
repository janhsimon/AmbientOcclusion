#pragma once

#include "Input.hpp"
#include "Transform.hpp"
#include "Window.hpp"

class Camera : public Transform
{
private:
	static const float MOVEMENT_SPEED;
	
	static float mouseSensitivity;

	glm::mat4 viewMatrix, projectionMatrix;
	float nearClipPlane, farClipPlane;

	Window *window;
	float fov;
	bool throwAwayFirstMouseMove;

	void doKeyboardInput(const Input *input, float deltaTime);
	void doMouseLook(const Input *input);

public:
	Camera(const glm::vec3 &position, Window *window);

	void update(const Input *input, float deltaTime);
	glm::vec4 projectPointToScreenSpace(const glm::vec4 &point) const;
	glm::vec4 projectPointToScreenSpace(const glm::vec3 &point) const;

	inline glm::mat4 getViewMatrix() const { return viewMatrix; }
	inline glm::mat4 getProjectionMatrix() const { return projectionMatrix; }
	inline float getFOV() const { return fov; }
	void setFOV(float fov);
};