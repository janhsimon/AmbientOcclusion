#include <assimp\postprocess.h>
//#include <gtc\quaternion.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtx\quaternion.hpp>
#include <sstream>

#include "Model.hpp"
#include "Error.hpp"

Assimp::Importer Model::importer = Assimp::Importer();

aiMatrix4x4 glmMat4ToAi(const glm::mat4 &from)
{
	aiMatrix4x4 to;

	to.a1 = from[0][0]; to.b1 = from[0][1]; to.c1 = from[0][2]; to.d1 = from[0][3];
	to.a2 = from[1][0]; to.b2 = from[1][1]; to.c2 = from[1][2]; to.d2 = from[1][3];
	to.a3 = from[2][0]; to.b3 = from[2][1]; to.c3 = from[2][2]; to.d3 = from[2][3];
	to.a4 = from[3][0]; to.b4 = from[3][1]; to.c4 = from[3][2]; to.d4 = from[3][3];

	return to;
}

Model::Model(glm::vec3 position) : Transform(position)
{
	animationTimer = 0.0f;
	animationSpeed = 20.0f;
	currentAnimation = 0;
	currentKey = 0;
	numAnimations = 0;
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

	rootNode = model->mRootNode;
	assert(rootNode);

	animations = model->mAnimations;
	assert(animations);

	numAnimations = model->mNumAnimations;

	for (unsigned int meshIndex = 0; meshIndex < model->mNumMeshes; ++meshIndex)
	// loop through each mesh in the model
	{
		Mesh *mesh = new Mesh(rootNode);

		if (!mesh->load(model->mMeshes[meshIndex]))
		{
			Error::report("Failed to load model \"" + filename + "\".");
			return false;
		}

		meshes.push_back(mesh);
	}

	return true;
}

void Model::changeAnimation()
{
	++currentAnimation;

	if (currentAnimation >= numAnimations)
		currentAnimation = 0;
}

void Model::update(float delta)
{
	assert(animations);
	aiAnimation *animation = animations[currentAnimation];
	assert(animation);

	if (animationTimer > animationSpeed)
	{
		++currentKey;

		if (currentKey >= animation->mChannels[0]->mNumPositionKeys)
			currentKey = 0;

		animationTimer = 0.0f;
	}
	else
		animationTimer += delta;

	for (unsigned int channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
	// loop through each node that is affected by this animation
	{
		aiNodeAnim *channel = animation->mChannels[channelIndex];
		assert(channel);

		assert(rootNode);
		aiNode *animationNode = rootNode->FindNode(channel->mNodeName);
		assert(animationNode);

		glm::vec3 translationVector = glm::vec3(channel->mPositionKeys[currentKey].mValue.x, channel->mPositionKeys[currentKey].mValue.y, channel->mPositionKeys[currentKey].mValue.z);
		glm::quat rotationQuaternion = glm::quat(channel->mRotationKeys[currentKey].mValue.w, channel->mRotationKeys[currentKey].mValue.x, channel->mRotationKeys[currentKey].mValue.y, channel->mRotationKeys[currentKey].mValue.z);
		glm::vec3 scaleVector = glm::vec3(channel->mScalingKeys[currentKey].mValue.x, channel->mScalingKeys[currentKey].mValue.y, channel->mScalingKeys[currentKey].mValue.z);

		glm::mat4 translation = glm::translate(glm::mat4(), translationVector);
		glm::mat4 rotation = glm::toMat4(rotationQuaternion);
		glm::mat4 scale = glm::scale(glm::mat4(), scaleVector);

		animationNode->mTransformation = glmMat4ToAi(translation * rotation * scale);
	}

	for (Mesh *m : meshes)
		m->update(delta);

	updateTransform(delta);
}

void Model::render(ForwardShader *forwardShader, Camera *camera)
{
	assert(forwardShader);
	glUseProgram(forwardShader->getProgram());
	assert(camera);
	forwardShader->setWorldViewProjectionUniforms(getWorldMatrix(), camera->projectionMatrix * camera->viewMatrix);

	for (Mesh *m : meshes)
		m->render(forwardShader);
}