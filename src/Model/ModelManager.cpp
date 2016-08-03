#include <cassert>

#include "ModelManager.hpp"

ModelManager::~ModelManager()
{
	if (unitQuadModel)
		delete unitQuadModel;

	if (testSphereModel)
		delete testSphereModel;

	for (auto model : models)
		delete model;

	for (auto animatedModel : animatedModels)
		delete animatedModel;
}

bool ModelManager::load()
{
	unitQuadModel = new Model();
	if (!unitQuadModel->load("Models\\UnitQuad.obj", aiColor4D(0.0f, 1.0f, 0.0f, 0.5f)))
		return false;

	testSphereModel = new Model();
	if (!testSphereModel->load("Models\\UnitSphere.obj", aiColor4D(1.0f, 0.5f, 0.0f, 1.0f)))
		return false;

	testSphereModel->setPosition(glm::vec3(-1000.0f, 100.0f, 0.0f));

	return true;
}

bool ModelManager::loadModel(const std::string &filename)
{
	Model *model = new Model();

	if (!model->load(filename))
		return false;

	models.push_back(model);
	return true;
}

bool ModelManager::loadAnimatedModel(const std::string &filename)
{
	AnimatedModel *animatedModel = new AnimatedModel();

	if (!animatedModel->load(filename))
		return false;

	animatedModels.push_back(animatedModel);
	return true;
}