#pragma once

#include <vector>

#include "DirectionalLight.hpp"

class LightManager
{
private:
	std::vector<DirectionalLight*> directionalLights;
	//std::vector<PointLight*> directionalLights;

public:
	bool addDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color);

	inline size_t getNumDirectionalLights() const { return directionalLights.size(); }
	//inline size_t getNumPointLights() const { return pointLights.size(); }

	inline DirectionalLight* getDirectionalLightAt(unsigned int index) const { assert(index < directionalLights.size()); return directionalLights[index]; }
	//inline PointLight* getPointLightAt(unsigned int index) const { assert(index < pointLights.size()); return pointLights[index]; }
};