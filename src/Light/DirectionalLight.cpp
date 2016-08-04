#include "DirectionalLight.hpp"

DirectionalLight::DirectionalLight(const glm::vec3 &direction, const glm::vec3 &color)
{
	Transform::Transform();
	forward = direction;

	this->color = color;
}