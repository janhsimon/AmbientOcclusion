#include <gtc\matrix_transform.hpp>
#include <gtc\quaternion.hpp>

#include "Transform.hpp"

Transform::Transform(glm::vec3 position)
{
	this->position = position;
	velocity = glm::vec3(0.f, 0.f, 0.f);
	acceleration = glm::vec3(0.f, 0.f, 0.f);

	pitch = yaw = roll = 0.f;

	forward	= glm::vec3(0.0f, 0.0f, 1.0f);
	right	= glm::vec3(1.0f, 0.0f, 0.0f);
	up		= glm::vec3(0.0f, 1.0f, 0.0f);
}

void Transform::updateTransform(float delta)
{
	velocity += acceleration * delta;
	//velocity *= 0.1f * delta;
	position += velocity * delta;
	
	glm::mat4 orientationMatrix = glm::mat4_cast(glm::fquat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll))));

	forward = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.f)));
	right = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.f)));
	up = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f)));
}

glm::mat4 Transform::getWorldMatrix() const
{
	glm::mat4 worldMatrix(1.f);

	worldMatrix = glm::translate(worldMatrix, position);

	worldMatrix = glm::rotate(worldMatrix, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));

	return worldMatrix;
}

void Transform::setPitch(float pitch)
{
	this->pitch = pitch;
	updateTransform(0.f);
}

void Transform::setYaw(float yaw)
{
	this->yaw = yaw;
	updateTransform(0.f);
}

void Transform::setRoll(float roll)
{
	this->roll = roll;
	updateTransform(0.f);
}