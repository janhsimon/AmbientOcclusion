#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <glew.h>
#include <glm.hpp>

#include "Camera.hpp"
#include "ForwardShader.hpp"
#include "Mesh.hpp"
#include "Transform.hpp"

class Model : public Transform
{
private:
	static Assimp::Importer importer;

	std::vector<Mesh*> meshes;
	aiNode *rootNode;
	aiAnimation **animations;

	float animationTimer;
	float animationSpeed;
	unsigned int currentAnimation;
	unsigned int currentKey;
	unsigned int numAnimations;

public:
	Model(glm::vec3 position = glm::vec3(0.0f));
	
	bool load(const std::string &filename);
	void changeAnimation();
	void update(float delta);
	void render(ForwardShader *forwardShader, Camera *camera);
};