#pragma once

#include <vector>

#include "AnimatedModel.hpp"
#include "Model.hpp"

class ModelManager
{
private:
	std::vector<Model*> models;
	std::vector<AnimatedModel*> animatedModels;
	Model *unitQuadModel;
	Model *testSphereModel;

public:
	~ModelManager();

	bool load();

	bool loadModel(const std::string &filename);
	bool loadAnimatedModel(const std::string &filename);
	
	inline size_t getNumModels() const { return models.size(); }
	inline size_t getNumAnimatedModels() const { return models.size(); }

	inline Model* getModelAt(unsigned int index) const { assert(index < models.size()); return models[index]; }
	inline Model* getAnimatedModelAt(unsigned int index) const { assert(index < animatedModels.size()); return animatedModels[index]; }

	inline Model* getUnitQuadModel() const { assert(unitQuadModel); return unitQuadModel; }
	inline Model* getTestSphereModel() const { assert(testSphereModel); return testSphereModel; }
};