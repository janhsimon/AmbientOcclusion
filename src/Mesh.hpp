#pragma once

#include <assimp\mesh.h>
#include <vector>

struct Vertex
{
	float positionX, positionY, positionZ;
	float u, v;
	float normalX, normalY, normalZ;
	float tangentX, tangentY, tangentZ;
	float bitangentX, bitangentY, bitangentZ;
};

class Mesh
{
public:
	unsigned int vertexCount;
	//unsigned int materialIndex;

	std::vector<Vertex> load(aiMesh *mesh);
	void render(unsigned int firstVertex);
};