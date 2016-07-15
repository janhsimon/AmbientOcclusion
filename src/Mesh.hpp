#pragma once

#include <assimp\mesh.h>
#include <vector>

struct Vertex
{
	float positionX, positionY, positionZ;
	float colorR, colorG, colorB, colorA;
	float normalX, normalY, normalZ;
};

class Mesh
{
private:
	GLuint VAO, VBO, IBO;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void populateVertices(const aiMesh *mesh);
	void populateIndices(const aiMesh *mesh);

public:
	Mesh();
	~Mesh();

	bool load(const aiMesh *mesh);
	void render() const;
};