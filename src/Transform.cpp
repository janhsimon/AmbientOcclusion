#include <gtc\matrix_transform.hpp>
#include <gtx\quaternion.hpp>
#include <gtx\rotate_vector.hpp>

#include "Transform.hpp"

Transform::Transform(const glm::vec3 &position)
{
	this->position = position;

	velocity		= glm::vec3(0.0f, 0.0f, 0.0f);
	acceleration	= glm::vec3(0.0f, 0.0f, 0.0f);
	scale			= glm::vec3(1.0f, 1.0f, 1.0f);

	useYawPitch = true;
}

void Transform::update(float deltaTime)
{
	velocity += acceleration * deltaTime;
	position += velocity * deltaTime;

	if (useYawPitch)
	{
		forward.x = sin(yaw) * cos(pitch);
		forward.y = sin(pitch);
		forward.z = -cos(yaw) * cos(pitch);
		forward = glm::normalize(forward);

		right = glm::normalize(glm::cross(forward, glm::vec3(0.0, 1.0f, 0.0f)));
		up = glm::normalize(glm::cross(right, forward));
	}

	worldMatrix = glm::mat4(1.0f);

	worldMatrix[0][0] = right.x;
	worldMatrix[0][1] = right.y;
	worldMatrix[0][2] = right.z;

	worldMatrix[1][0] = up.x;
	worldMatrix[1][1] = up.y;
	worldMatrix[1][2] = up.z;

	worldMatrix[2][0] = forward.x;
	worldMatrix[2][1] = forward.y;
	worldMatrix[2][2] = forward.z;

	worldMatrix = glm::translate(worldMatrix, position);
	worldMatrix = glm::scale(worldMatrix, scale);
}

void Transform::rotateYaw(float degrees)
{
	yaw += degrees;
}

void Transform::rotatePitch(float degrees)
{
	pitch += degrees;

	const float limit = glm::radians(89.0f);

	if (pitch > limit)
		pitch = limit;
	else if (pitch < -limit)
		pitch = -limit;
}