#include "LightManager.hpp"
#include "..\Error.hpp"

bool LightManager::addDirectionalLight(const glm::vec3 &direction, const glm::vec3 &color)
{
	DirectionalLight *directionalLight;
	if (!Error::checkMemory(directionalLight = new DirectionalLight(direction, color)))	return false;

	assert(directionalLight);
	directionalLights.push_back(directionalLight);
	return true;
}