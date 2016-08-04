#include <cassert>

#include "ModelManager.hpp"
#include "..\Error.hpp"

ModelManager::~ModelManager()
{
	if (directionalLightGeometry) delete directionalLightGeometry;
	if (pointLightGeometry) delete pointLightGeometry;

	for (auto model : models) delete model;
	for (auto animatedModel : animatedModels) delete animatedModel;
}

bool ModelManager::load()
{
	if (!Error::checkMemory(directionalLightGeometry = new Model())) return false;
	if (!directionalLightGeometry->load("Models\\DirectionalLightGeometry.obj")) return false;

	if (!Error::checkMemory(pointLightGeometry = new Model())) return false;
	if (!pointLightGeometry->load("Models\\PointLightGeometry.obj")) return false;

	return true;
}

bool ModelManager::loadModel(const std::string &filename, const glm::vec3 &position)
{
	Model *model;
	if (!Error::checkMemory(model = new Model(position))) return false;

	if (!model->load(filename))
		return false;

	models.push_back(model);
	return true;
}

bool ModelManager::loadAnimatedModel(const std::string &filename, const glm::vec3 &position)
{
	AnimatedModel *animatedModel;
	if (!Error::checkMemory(animatedModel = new AnimatedModel(position))) return false;

	if (!animatedModel->load(filename))
		return false;

	animatedModels.push_back(animatedModel);
	return true;
}