#include <assimp\scene.h>
#include <cassert>
#include <glew.h>
#include <sstream>

#include "Error.hpp"
#include "Mesh.hpp"

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
{
	glm::mat4 to;

	to[0][0] = (GLfloat)from.a1; to[0][1] = (GLfloat)from.b1;  to[0][2] = (GLfloat)from.c1; to[0][3] = (GLfloat)from.d1;
	to[1][0] = (GLfloat)from.a2; to[1][1] = (GLfloat)from.b2;  to[1][2] = (GLfloat)from.c2; to[1][3] = (GLfloat)from.d2;
	to[2][0] = (GLfloat)from.a3; to[2][1] = (GLfloat)from.b3;  to[2][2] = (GLfloat)from.c3; to[2][3] = (GLfloat)from.d3;
	to[3][0] = (GLfloat)from.a4; to[3][1] = (GLfloat)from.b4;  to[3][2] = (GLfloat)from.c4; to[3][3] = (GLfloat)from.d4;

	return to;
}

Mesh::Mesh(aiNode *rootNode)
{
	this->rootNode = rootNode;

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

		Vertex newVertex = { position.x, position.y, position.z, color.r, color.g, color.b, color.a, normal.x, normal.y, normal.z, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, 0, 0 };
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

void Mesh::populateBones(const aiMesh *mesh)
{
	bones.clear();

	assert(mesh);
	if (!mesh->HasBones())
		return;

	for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	// loop through each bone
	{
		const aiBone *bone = mesh->mBones[boneIndex];
		assert(bone);
		
		for (unsigned int vertexWeightIndex = 0; vertexWeightIndex < bone->mNumWeights; ++vertexWeightIndex)
		// loop through each vertex that is affected by this bone
		{
			const aiVertexWeight vertexWeight = bone->mWeights[vertexWeightIndex];
			Vertex &affectedVertex = vertices[vertexWeight.mVertexId];
			float newWeight = vertexWeight.mWeight;

			for (int i = 0; i < 4; ++i)
			{
				if (newWeight > affectedVertex.boneWeight[i])
				{
					affectedVertex.boneWeight[i] = newWeight;
					affectedVertex.boneID[i] = boneIndex;
					break;
				}
			}
		}

		Bone *newBone = new Bone();

		newBone->inverseBindMatrix = aiMatrix4x4ToGlm(bone->mOffsetMatrix);

		assert(rootNode);
		aiNode *boneNode = rootNode->FindNode(bone->mName);
		assert(boneNode);
		newBone->node = boneNode;

		bones.push_back(newBone);
	}
}

bool Mesh::load(const aiMesh *mesh)
{
	assert(mesh);
	populateVertices(mesh);
	populateIndices(mesh);
	populateBones(mesh);

	if (vertices.size() <= 0 || indices.size() <= 0)
		return false;

	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 0));	// position
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));	// color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 7));	// normal
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 10));	// bone weights
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT,   sizeof(Vertex), (void *)(sizeof(float) * 14));	// bone ids

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	if (glGetError() != GL_NO_ERROR)
		return false;

	return true;
}

void Mesh::update(float delta)
{
	assert(rootNode);

	for (Bone *bone : bones)
	{
		aiNode *boneNode = bone->node;
		assert(boneNode);
		
		aiMatrix4x4 boneMatrix;
		do
		{
			boneMatrix = boneNode->mTransformation * boneMatrix;
			boneNode = boneNode->mParent;
		}
		while (boneNode);

		bone->boneMatrix = aiMatrix4x4ToGlm(boneMatrix);
	}
}

void Mesh::render(ForwardShader *forwardShader) const
{
	// create a snapshot of the current bone matrices
	//std::vector<glm::mat4> boneMatrices;
	float *boneMatrices = new float[64 * 16];
	float *boneMatricesIT = new float[64 * 9];
	for (unsigned int boneMatrixIndex = 0; boneMatrixIndex < 64; ++boneMatrixIndex)
	{
		glm::mat4 totalMatrix = glm::mat4();
		
		if (boneMatrixIndex < bones.size())
			totalMatrix = bones[boneMatrixIndex]->boneMatrix * bones[boneMatrixIndex]->inverseBindMatrix;

		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				boneMatrices[16 * boneMatrixIndex + i * 4 + j] = totalMatrix[i][j];

		glm::mat3 totalMatrixIT = glm::mat3(totalMatrix);
		totalMatrixIT = glm::transpose(glm::inverse(totalMatrixIT));

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				boneMatricesIT[9 * boneMatrixIndex + i * 3 + j] = totalMatrixIT[i][j];
	}

	assert(forwardShader);
	forwardShader->setBoneMatrixUniforms(boneMatrices);
	forwardShader->setBoneMatrixITUniforms(boneMatricesIT);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
}