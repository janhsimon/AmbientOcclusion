#pragma once

#include <glew.h>
#include <glm.hpp>
#include <vector>
#include <assimp\Importer.hpp>

#include "Bone.hpp"
#include "Vertex.hpp"
#include "..\Transform.hpp"

class Model : public Transform
{
private:
	static Assimp::Importer importer;

protected:
	GLuint VAO, VBO, IBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	
	const aiScene *readFile(const std::string &filename);
	unsigned int loadVertices(const aiMesh *mesh);
	void loadIndices(const aiMesh *mesh, unsigned int startingVertex);

public:
	Model(const glm::vec3 &position = glm::vec3(0.0f));
	~Model();

	bool load(const std::string &filename);
	void update(float delta);
	void render();
};