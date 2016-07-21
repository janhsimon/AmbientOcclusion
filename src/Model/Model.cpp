#include <assimp\postprocess.h>

#include "Model.hpp"
#include "..\Error.hpp"

Assimp::Importer Model::importer = Assimp::Importer();

Model::Model(const glm::vec3 &position) : Transform(position)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
}

Model::~Model()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

const aiScene *Model::readFile(const std::string &filename)
{
	const aiScene *model = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (!model)
	// if there was an issue loading the model
	{
		Error::report("Failed to load model \"" + filename + "\": " + importer.GetErrorString());
		return nullptr;
	}

	return model;
}

unsigned int Model::loadVertices(const aiMesh *mesh)
{
	unsigned int startingVertex = (unsigned int)vertices.size();

	assert(mesh);
	for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
	// loop through each vertex in the mesh
	{
		const aiVector3D position = mesh->HasPositions() ? mesh->mVertices[vertexIndex] : aiVector3D(0.0f, 0.0f, 0.0f);
		const aiColor4D color = mesh->HasVertexColors(0) ? mesh->mColors[0][vertexIndex] : aiColor4D(0.7f, 0.7f, 0.7f, 1.0f);
		const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[vertexIndex] : aiVector3D(0.0f, 1.0f, 0.0f);

		vertices.push_back(Vertex(position, normal, color));
	}

	return startingVertex;
}

void Model::loadIndices(const aiMesh *mesh, unsigned int startingVertex)
{
	assert(mesh);
	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	// loop through each face in the mesh
	{
		const aiFace face = mesh->mFaces[faceIndex];

		if (face.mNumIndices != 3)
		// we expect triangles only
			continue;

		for (int i = 0; i < 3; ++i)
		// loop through each index in this face
			// we use the starting vertex number here to offset our indices correctly
			indices.push_back(startingVertex + face.mIndices[i]);
	}
}

bool Model::load(const std::string &filename)
{
	const aiScene *model = readFile(filename);
	if (!model)
		return false;

	vertices.clear();
	indices.clear();

	for (unsigned int meshIndex = 0; meshIndex < model->mNumMeshes; ++meshIndex)
	// loop through each mesh in the model
	{
		const aiMesh *mesh = model->mMeshes[meshIndex];
		assert(mesh);
		
		unsigned int startingVertex = loadVertices(mesh);
		loadIndices(mesh, startingVertex);
	}

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 0));	// position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));	// normal
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 6));	// color

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// make sure these can not be changed from the outside
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (glGetError() != GL_NO_ERROR)
	// if there was an issue loading the model
	{
		Error::report("Failed to load model \"" + filename + "\".");
		return false;
	}

	return true;
}

void Model::update(float delta)
{
	Transform::update(delta);
}

void Model::render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
}