#include <gtc\matrix_transform.hpp>
#include <gtx\quaternion.hpp>

#include "Transform.hpp"

Transform::Transform(glm::vec3 position)
{
	this->position = position;
	velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

	pitch = yaw = roll = 0.0f;

	forward	= glm::vec3(0.0f, 0.0f, 1.0f);
	right	= glm::vec3(1.0f, 0.0f, 0.0f);
	up		= glm::vec3(0.0f, 1.0f, 0.0f);

	scale = 1.0f;
}

void Transform::update(float delta)
{
	velocity += acceleration * delta;
	//velocity *= 0.1f * delta;
	position += velocity * delta;
	
	//glm::mat4 orientationMatrix = glm::mat4_cast(glm::fquat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll))));
	glm::mat4 orientationMatrix = glm::toMat4(glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), glm::radians(roll))));

	forward = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(0.f, 0.f, 1.f, 0.f)));
	right = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(1.f, 0.f, 0.f, 0.f)));
	up = glm::normalize(glm::vec3(orientationMatrix * glm::vec4(0.f, 1.f, 0.f, 0.f)));
}

glm::mat4 Transform::getWorldMatrix() const
{
	glm::mat4 worldMatrix(1.0f);

	worldMatrix = glm::translate(worldMatrix, position);

	worldMatrix = glm::scale(worldMatrix, glm::vec3(scale, scale, scale));

	worldMatrix = glm::rotate(worldMatrix, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
	worldMatrix = glm::rotate(worldMatrix, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));

	return worldMatrix;
}

void Transform::setPitch(float pitch)
{
	this->pitch = pitch;
	update(0.0f);
}

void Transform::setYaw(float yaw)
{
	this->yaw = yaw;
	update(0.0f);
}

void Transform::setRoll(float roll)
{
	this->roll = roll;
	update(0.0f);
}

void Transform::setPitchYawRoll(float pitch, float yaw, float roll)
{
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
	update(0.0f);
}