#pragma once

#include <vector>

#include "AnimatedModel.hpp"
#include "Model.hpp"

class ModelManager
{
private:
	std::vector<Model*> models;
	std::vector<AnimatedModel*> animatedModels;
	Model *directionalLightGeometry;
	Model *pointLightGeometry;

public:
	~ModelManager();

	bool load();

	bool loadModel(const std::string &filename, const glm::vec3 &position = glm::vec3(0.0f));
	bool loadAnimatedModel(const std::string &filename, const glm::vec3 &position = glm::vec3(0.0f));
	
	inline size_t getNumModels() const { return models.size(); }
	inline size_t getNumAnimatedModels() const { return models.size(); }

	inline Model* getModelAt(unsigned int index) const { assert(index < models.size()); return models[index]; }
	inline Model* getAnimatedModelAt(unsigned int index) const { assert(index < animatedModels.size()); return animatedModels[index]; }

	inline Model* getDirectionalLightGeometry() const { assert(directionalLightGeometry); return directionalLightGeometry; }
	inline Model* getPointLightGeometry() const { assert(pointLightGeometry); return pointLightGeometry; }
};