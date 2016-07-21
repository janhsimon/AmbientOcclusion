#pragma once

#include <glm.hpp>

class Transform
{
protected:
	glm::vec3 position, velocity, acceleration;
	glm::vec3 forward, right, up;
	float pitch, yaw, roll;
	float scale;

	void update(float delta);

public:
	Transform(glm::vec3 position = glm::vec3(0.0f));

	glm::mat4 getWorldMatrix() const;

	inline glm::vec3 getPosition() const { return position; }
	inline void setPosition(const glm::vec3 &position) { this->position = position; }

	inline float getPitch() const { return pitch; }
	void setPitch(float pitch);

	inline float getYaw() const { return yaw; }
	void setYaw(float yaw);

	inline float getRoll() const { return roll; }
	void setRoll(float roll);

	void setPitchYawRoll(float pitch, float yaw, float roll);

	inline float getScale() const { return scale; }
	inline void setScale(float scale) { this->scale = scale; }

	inline glm::vec3 getForward() const { return forward; }
	inline glm::vec3 getRight() const { return right; }
	inline glm::vec3 getUp() const { return up; }
};