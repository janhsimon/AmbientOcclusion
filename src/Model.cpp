#include <assimp\postprocess.h>

#include "Model.hpp"
#include "Error.hpp"

Assimp::Importer Model::importer = Assimp::Importer();

Model::Model(glm::vec3 position) : Transform(position)
{

}

bool Model::load(const std::string &filename)
{
	const aiScene *model = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenNormals);

	if (!model)
	// if there was an issue loading the model
	{
		Error::report("Failed to load model \"" + filename + "\": " + importer.GetErrorString());
		return false;
	}

	for (unsigned int meshIndex = 0; meshIndex < model->mNumMeshes; ++meshIndex)
	// loop through each mesh in the model
	{
		Mesh *mesh = new Mesh();

		if (!mesh->load(model->mMeshes[meshIndex]))
		{
			Error::report("Failed to load model \"" + filename + "\".");
			return false;
		}

		meshes.push_back(mesh);
	}

	return true;
}

void Model::render()
{
	for (Mesh *m : meshes)
		m->render();
}