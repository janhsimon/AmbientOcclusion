#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <glew.h>
#include <glm.hpp>

#include "Mesh.hpp"
#include "Transform.hpp"

class Model : public Transform
{
private:
	static Assimp::Importer importer;

	std::vector<Mesh*> meshes;

public:
	Model(glm::vec3 position = glm::vec3(0.0f));
	
	bool load(const std::string &filename);
	void render();
};