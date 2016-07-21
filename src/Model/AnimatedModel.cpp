#include <gtc\matrix_transform.hpp>
#include <gtx\quaternion.hpp>

#include "AnimatedModel.hpp"
#include "MathHelper.hpp"
#include "..\Error.hpp"

const unsigned int AnimatedModel::MAX_BONES = 100;

AnimatedModel::AnimatedModel(const glm::vec3 &position) : Model(position)
{
	finalBoneMatrices = new glm::mat4[MAX_BONES];
	animationTimer = 0.0f;
	animationSpeed = 20.0f;
	currentAnimation = 0;
	currentKey = 0;
	numAnimations = 0;
}

AnimatedModel::~AnimatedModel()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void AnimatedModel::loadBones(const aiMesh *mesh, unsigned int startingVertex)
{
	assert(mesh);
	for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	// loop through each vertex in the mesh
	{
		// store the bone itself
		const aiBone *bone = mesh->mBones[boneIndex];
		assert(bone);
		assert(rootNode);
		aiNode *boneNode = rootNode->FindNode(bone->mName);
		assert(boneNode);
		bones.push_back(Bone(MathHelper::aiMatrix4x4ToGlm(bone->mOffsetMatrix), rootNode, boneNode));

		// and change the vertices so that they include our bone info (in case they are affected by it)
		for (unsigned int vertexWeightIndex = 0; vertexWeightIndex < bone->mNumWeights; ++vertexWeightIndex)
		// loop through each vertex weight in this bone
		{
			const aiVertexWeight vertexWeight = bone->mWeights[vertexWeightIndex];
			const unsigned int vertexID = startingVertex + vertexWeight.mVertexId;
			assert(vertexID < vertices.size());
			Vertex &vertex = vertices[vertexID];

			for (unsigned int i = 0; i < 4; ++i)
			// TODO: this is not optimal as it can push new bones into the first slot over and over, find a better way
			{
				if (vertexWeight.mWeight > vertex.getBoneWeight(i))
				// if our bone is affecting this vertex stronger than the previous one
				{
					vertex.setBoneInfo(i, (unsigned int)bones.size() - 1, vertexWeight.mWeight);
					break;
				}
			}
		}
	}
}

bool AnimatedModel::load(const std::string &filename)
{
	const aiScene *model = Model::readFile(filename);
	if (!model)
		return false;

	rootNode = model->mRootNode;
	assert(rootNode);

	animations = model->mAnimations;
	assert(animations);

	numAnimations = model->mNumAnimations;

	vertices.clear();
	indices.clear();
	bones.clear();

	for (unsigned int meshIndex = 0; meshIndex < model->mNumMeshes; ++meshIndex)
	// loop through each mesh in the model
	{
		const aiMesh *mesh = model->mMeshes[meshIndex];
		assert(mesh);

		unsigned int startingVertex = loadVertices(mesh);
		loadIndices(mesh, startingVertex);
		loadBones(mesh, startingVertex);
	}
	
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 0));  // position
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));  // normal
	glVertexAttribPointer (2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 6));  // color
	glVertexAttribPointer (3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 10)); // bone weights
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT,    sizeof(Vertex), (void *)(sizeof(float) * 14)); // bone ids

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

void AnimatedModel::changeAnimation()
{
	++currentAnimation;

	if (currentAnimation >= numAnimations)
		currentAnimation = 0;
}

void AnimatedModel::update(float delta)
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
	// loop through each channel that is affected by this animation
	{
		aiNodeAnim *channel = animation->mChannels[channelIndex];
		assert(channel);

		glm::vec3 translationVector = glm::vec3(channel->mPositionKeys[currentKey].mValue.x, channel->mPositionKeys[currentKey].mValue.y, channel->mPositionKeys[currentKey].mValue.z);
		glm::quat rotationQuaternion = glm::quat(channel->mRotationKeys[currentKey].mValue.w, channel->mRotationKeys[currentKey].mValue.x, channel->mRotationKeys[currentKey].mValue.y, channel->mRotationKeys[currentKey].mValue.z);
		glm::vec3 scaleVector = glm::vec3(channel->mScalingKeys[currentKey].mValue.x, channel->mScalingKeys[currentKey].mValue.y, channel->mScalingKeys[currentKey].mValue.z);

		glm::mat4 translation = glm::translate(glm::mat4(), translationVector);
		glm::mat4 rotation = glm::toMat4(rotationQuaternion);
		glm::mat4 scale = glm::scale(glm::mat4(), scaleVector);

		assert(rootNode);
		aiNode *animationNode = rootNode->FindNode(channel->mNodeName);
		assert(animationNode);
		animationNode->mTransformation = MathHelper::glmMat4ToAi(translation * rotation * scale);
	}

	/*
	for (unsigned int boneIndex = 0; boneIndex < MAX_BONES; ++boneIndex)
	// loop through each bone
	{
		if (boneIndex < bones.size())
			finalBoneMatrices[boneIndex] = bones[boneIndex].getFinalMatrix();
	}
	*/

	Model::update(delta);
}

void AnimatedModel::render(SkinnedForwardShader *skinnedForwardShader)
{
	float *data = new float[MAX_BONES * 16];

	for (int i = 0; i < MAX_BONES; ++i)
	{
		glm::mat4 totalMatrix = glm::mat4();

		if (i < bones.size())
			totalMatrix = bones[i].getFinalMatrix();

		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				const unsigned int index = i * 16 + j * 4 + k;
				data[index] = totalMatrix[j][k];//finalBoneMatrices[i][j][k];
			}
		}
	}

	assert(skinnedForwardShader);
	skinnedForwardShader->setBoneMatrixUniforms(data, MAX_BONES);
	delete data;

	Model::render();
}