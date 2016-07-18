#pragma once

#include <assimp\mesh.h>
#include <glm.hpp>
#include <vector>

#include "ForwardShader.hpp"

struct Vertex
{
	float position[3];
	float color[4];
	float normal[3];
	float boneWeight[4];
	unsigned int boneID[4];
};

struct Bone
{
	glm::mat4 inverseBindMatrix;
	glm::mat4 boneMatrix;
	aiNode *node;
};

class Mesh
{
private:
	GLuint VAO, VBO, IBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Bone*> bones;
	
	aiNode *rootNode;

	void populateVertices(const aiMesh *mesh);
	void populateIndices(const aiMesh *mesh);
	void populateBones(const aiMesh *mesh);

public:
	Mesh(aiNode *rootNode);
	~Mesh();

	bool load(const aiMesh *mesh);
	void update(float delta);
	void render(ForwardShader *forwardShader) const;
};