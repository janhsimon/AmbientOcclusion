#include <cassert>

#include "ModelManager.hpp"
#include "..\Error.hpp"

ModelManager::~ModelManager()
{
	if (unitQuad) delete unitQuad;
	if (pointLightVolumeModel) delete pointLightVolumeModel;
	if (testSphereModel) delete testSphereModel;

	for (auto model : models) delete model;
	for (auto animatedModel : animatedModels) delete animatedModel;
}

bool ModelManager::load()
{
	if (!Error::checkMemory(unitQuad = new Model())) return false;
	if (!unitQuad->load("Models\\UnitQuad.obj")) return false;

	if (!Error::checkMemory(pointLightVolumeModel = new Model())) return false;
	if (!pointLightVolumeModel->load("Models\\PointLightVolume.obj", aiColor3D(0.0f, 1.0f, 0.0f))) return false;

	if (!Error::checkMemory(testSphereModel = new Model())) return false;
	if (!testSphereModel->load("Models\\UnitSphere.obj", aiColor3D(1.0f, 0.0f, 0.0f))) return false;

	testSphereModel->setPosition(glm::vec3(-1000.0f, 100.0f, 0.0f));
	testSphereModel->setScale(glm::vec3(64.0f, 64.0f, 64.0f));
	testSphereModel->update(0.0f);

	return true;
}

bool ModelManager::loadModel(const std::string &filename)
{
	Model *model;
	if (!Error::checkMemory(model = new Model())) return false;

	if (!model->load(filename))
		return false;

	models.push_back(model);
	return true;
}

bool ModelManager::loadAnimatedModel(const std::string &filename)
{
	AnimatedModel *animatedModel;
	if (!Error::checkMemory(animatedModel = new AnimatedModel())) return false;

	if (!animatedModel->load(filename))
		return false;

	animatedModels.push_back(animatedModel);
	return true;
}