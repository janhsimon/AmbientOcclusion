#pragma once

#include <glm.hpp>

class Transform
{
protected:
	glm::vec3 position, velocity, acceleration;
	glm::vec3 forward, right, up;
	float yaw, pitch;
	glm::vec3 scale;
	glm::mat4 worldMatrix;
	bool useYawPitch;

	void update(float deltaTime);

public:
	Transform(const glm::vec3 &position = glm::vec3(0.0f));
	
	void rotateYaw(float degrees);
	void rotatePitch(float degrees);
	void rotateRoll(float degrees);

	inline glm::vec3 getPosition() const { return position; }
	inline void setPosition(const glm::vec3 &position) { this->position = position; }
	inline glm::vec3 getForward() const { return forward; }
	inline void setForward(const glm::vec3 &forward) { this->forward = forward; }
	inline glm::vec3 getRight() const { return right; }
	inline void setRight(const glm::vec3 &right) { this->right = right; }
	inline glm::vec3 getUp() const { return up; }
	inline void setUp(const glm::vec3 &up) { this->up = up; }
	inline glm::vec3 getScale() const { return scale; }
	inline void setScale(float scale) { this->scale = glm::vec3(scale, scale, scale); }
	inline void setScale(glm::vec3 scale) { this->scale = scale; }
	inline glm::mat4 getWorldMatrix() const { return worldMatrix; }
	inline bool getUseYawPitch() const { return useYawPitch; }
	inline void setUseYawPitch(bool useYawPitch) { this->useYawPitch = useYawPitch; }
};