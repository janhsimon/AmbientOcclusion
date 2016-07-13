#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <glew.h>
#include <glm.hpp>
#include <vector>

#include "Mesh.hpp"
//#include "..\Material\Material.hpp"
#include "Transform.hpp"

class Model : public Transform
{
private:
	static Assimp::Importer importer;
	
	GLuint VBO, VAO;
	std::vector<Mesh*> meshes;
	//std::vector<Material*> materials;

	std::string isolateFilename(const std::string &filename);
	//bool parseMaterials(const aiScene *model, const std::string &filename);
	void parseVertices(const aiScene *model, std::vector<Vertex> *vertices);

public:
	Model(glm::vec3 position);
	~Model();

	bool load(const std::string &filename);
	void render();
};