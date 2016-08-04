#pragma once

#include "..\Transform.hpp"

class DirectionalLight : public Transform
{
private:
	glm::vec3 color;

public:
	DirectionalLight(const glm::vec3 &direction, const glm::vec3 &color);

	inline glm::vec3 getColor() const { return color; }
	
};