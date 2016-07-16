#include <cassert>
#include <glew.h>

#include "Mesh.hpp"

Mesh::Mesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}

Mesh::~Mesh()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Mesh::populateVertices(const aiMesh *mesh)
{
	vertices.clear();

	assert(mesh);
	for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
	// loop through each vertex
	{
		const aiVector3D	position	= mesh->HasPositions() ? mesh->mVertices[vertexIndex] : aiVector3D(0.0f, 0.0f, 0.0f);
		const aiColor4D		color		= mesh->HasVertexColors(0) ? mesh->mColors[0][vertexIndex] : (0.7f, 0.7f, 0.7f, 1.0f);
		const aiVector3D	normal		= mesh->HasNormals() ? mesh->mNormals[vertexIndex] : aiVector3D(0.0f, 1.0f, 0.0f);

		Vertex newVertex = { position.x, position.y, position.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z };
		vertices.push_back(newVertex);
	}
}

void Mesh::populateIndices(const aiMesh *mesh)
{
	indices.clear();

	assert(mesh);
	if (!mesh->HasFaces())
		return;

	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	// loop through each face
	{
		const aiFace face = mesh->mFaces[faceIndex];

		if (face.mNumIndices != 3)
		// we expect triangulated faces
			continue;

		for (unsigned int index = 0; index < face.mNumIndices; ++index)
		// loop through each index in this face
			indices.push_back(face.mIndices[index]);
	}
}

bool Mesh::load(const aiMesh *mesh)
{
	assert(mesh);
	populateVertices(mesh);
	populateIndices(mesh);

	if (vertices.size() <= 0 || indices.size() <= 0)
		return false;

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);								// position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));	// color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 7));	// normal

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	if (glGetError() != GL_NO_ERROR)
		return false;

	return true;
}

void Mesh::render() const
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
}