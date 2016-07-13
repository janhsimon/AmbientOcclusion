#include <cassert>
#include <glew.h>

#include "Mesh.hpp"

std::vector<Vertex> Mesh::load(aiMesh *mesh)
{
	std::vector<Vertex> vertices;

	// also take note of the material index
	//materialIndex = mesh->mMaterialIndex;

	for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
	// loop through each face in the mesh
	{
		const aiFace *face = &(mesh->mFaces[faceIndex]);
		assert(face);
		assert(face->mNumIndices <= 3);
		
		if (face->mNumIndices < 3)
			continue;

		for (int i = 0; i < 3; ++i)
		// we expect triangulated faces
		{
			unsigned int index = face->mIndices[i];

			aiVector3D pos = aiVector3D(0.f, 0.f, 0.f);

			if (mesh->HasPositions())
				pos = mesh->mVertices[index];

			aiVector3D uv = aiVector3D(0.f, 0.f, 0.f);

			if (mesh->HasTextureCoords(0))
				uv = mesh->mTextureCoords[0][index];

			aiVector3D normal = aiVector3D(0.f, 0.f, 0.f);

			if (mesh->HasNormals())
				normal = mesh->mNormals[index];

			aiVector3D tangent = aiVector3D(0.f, 0.f, 0.f);
			aiVector3D bitangent = aiVector3D(0.f, 0.f, 0.f);
			
			if (mesh->HasTangentsAndBitangents())
			{
				tangent = mesh->mTangents[index];
				bitangent = mesh->mBitangents[index];
			}

			// add this vertex to our list
			Vertex newVertex = { pos.x, pos.y, pos.z, uv.x, uv.y, normal.x, normal.y, normal.z, tangent.x, tangent.y, tangent.z, bitangent.x, bitangent.y, bitangent.z };
			vertices.push_back(newVertex);
		}
	}

	vertexCount = mesh->mNumFaces * 3;

	return vertices;
}

void Mesh::render(unsigned int firstVertex)
{
	glDrawArrays(GL_TRIANGLES, firstVertex, vertexCount);
}