#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <gtc\matrix_transform.hpp>
#include <gtx\quaternion.hpp>
#include <sstream>

#include "AnimatedModel.hpp"
#include "MathHelper.hpp"
#include "..\Error.hpp"

const unsigned int AnimatedModel::MAX_BONES = 100;

AnimatedModel::AnimatedModel(const glm::vec3 &position) : Model(position)
{
	currentAnimation = 0;
	if (!Error::checkMemory(finalBoneMatrices = new glm::mat4[MAX_BONES])) return;
}

AnimatedModel::~AnimatedModel()
{
	glDeleteBuffers(1, &IBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

Bone *AnimatedModel::getBoneFromName(const std::string &name)
{
	for (Bone *bone : bones)
	{
		if (bone->getName().compare(name) == 0)
			return bone;
	}

	return nullptr;
}

void AnimatedModel::loadBones(const aiScene *model, const aiMesh *mesh, unsigned int startingVertex)
{
	assert(mesh);
	for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	// loop through each bone in the model
	{
		// store the bone itself
		const aiBone *bone = mesh->mBones[boneIndex];
		assert(bone);
		assert(model->mRootNode);
		aiNode *boneNode = model->mRootNode->FindNode(bone->mName);
		assert(boneNode);
		glm::mat4 inverseBindPoseMatrix = MathHelper::aiMatrix4x4ToGlm(bone->mOffsetMatrix);
		Bone *newBone;
		if (!Error::checkMemory(newBone = new Bone(bone->mName.C_Str(), inverseBindPoseMatrix))) return;
		bones.push_back(newBone);

		std::stringstream s;
		s << "Bone \"" << bone->mName.C_Str() << "\" added.";
		//Error::report(s.str());

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

	// now lets identify parents
	for (Bone *bone : bones)
	{
		aiNode *node = model->mRootNode->FindNode(bone->getName().c_str());

		if (!node)
		{
			Error::report("Failed to find model node for bone \"" + bone->getName() + "\".");
			continue;
		}

		aiNode *parentNode = node->mParent;

		if (!parentNode)
			continue;

		Bone *parent = getBoneFromName(parentNode->mName.C_Str());
		bone->setParent(parent);
	}
}

void AnimatedModel::loadAnimations(const aiScene *model)
{
	assert(model);

	animations.clear();
	currentAnimation = 0;

	for (unsigned int animationIndex = 0; animationIndex < model->mNumAnimations; ++animationIndex)
	// loop through each animation in the model
	{
		const aiAnimation *animation = model->mAnimations[animationIndex];
		assert(animation);

		Animation newAnimation;

		for (unsigned int channelIndex = 0; channelIndex < animation->mNumChannels; ++channelIndex)
		// loop through each channel in the animation
		{
			const aiNodeAnim *channel = animation->mChannels[channelIndex];
			assert(channel);
			assert(model->mRootNode);
			Bone *affectedBone = getBoneFromName(channel->mNodeName.C_Str());
			
			if (!affectedBone)
			{
				std::stringstream s;
				s << "Failed to find affected bone \"" << channel->mNodeName.C_Str() << "\" for animation \"" << animation->mName.C_Str() << "\".";
				//Error::report(s.str());
				continue;
			}

			for (unsigned int positionKeyIndex = 0; positionKeyIndex < channel->mNumPositionKeys; ++positionKeyIndex)
			// loop through each position key in the channel
			{
				const aiVector3D &translationVector = channel->mPositionKeys[positionKeyIndex].mValue;
				glm::mat4 translation = glm::translate(glm::mat4(), glm::vec3(translationVector.x, translationVector.y, translationVector.z));
				newAnimation.addTranslationKey(affectedBone, translation);
			}
			
			for (unsigned int rotationKeyIndex = 0; rotationKeyIndex < channel->mNumRotationKeys; ++rotationKeyIndex)
			// loop through each rotation key in the channel
			{
				const aiQuaternion &rotationQuaternion = channel->mRotationKeys[rotationKeyIndex].mValue;
				glm::quat rotation = glm::quat(rotationQuaternion.w, rotationQuaternion.x, rotationQuaternion.y, rotationQuaternion.z);
				newAnimation.addRotationKey(affectedBone, rotation);
			}

			for (unsigned int scaleKeyIndex = 0; scaleKeyIndex < channel->mNumScalingKeys; ++scaleKeyIndex)
			// loop through each scale key in the channel
			{
				const aiVector3D &scaleVector = channel->mPositionKeys[scaleKeyIndex].mValue;
				glm::mat4 scale = glm::translate(glm::mat4(), glm::vec3(scaleVector.x, scaleVector.y, scaleVector.z));
				newAnimation.addScaleKey(affectedBone, scale);
			}
		}

		animations.push_back(newAnimation);
	}
}

bool AnimatedModel::load(const std::string &filename)
{
	Assimp::Importer importer;
	const aiScene *model = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals);

	if (!model)
	// if there was an issue loading the model
	{
		Error::report("Failed to load model \"" + filename + "\": " + importer.GetErrorString());
		return false;
	}

	if (model->mNumMeshes <= 0)
	{
		Error::report("Failed to load model \"" + filename + "\": No mesh data found.");
		return false;
	}

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
		loadBones(model, mesh, startingVertex);
	}

	if (vertices.size() <= 0)
	{
		Error::report("Failed to load model \"" + filename + "\": No vertex data found.");
		return false;
	}

	if (indices.size() <= 0)
	{
		Error::report("Failed to load model \"" + filename + "\": No index data found.");
		return false;
	}

	if (bones.size() <= 0)
	{
		Error::report("Failed to load model \"" + filename + "\": No bone data found.");
		return false;
	}

	if (model->mNumAnimations <= 0)
	{
		Error::report("Failed to load model \"" + filename + "\": No animation data found.");
		return false;
	}

	loadAnimations(model);
	
	glBindVertexArray(VAO);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 0));	// position
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 3));	// normal
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 6));	// color
	glVertexAttribPointer (3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(sizeof(float) * 9));	// bone weights
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT,    sizeof(Vertex), (void *)(sizeof(float) * 13));	// bone ids

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// make sure these can not be changed from the outside
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (!Error::checkGL())
		return false;

	return true;
}

void AnimatedModel::changeAnimation()
{
	++currentAnimation;

	if (currentAnimation >= animations.size())
		currentAnimation = 0;
}

void AnimatedModel::update(float deltaTime)
{
	assert(currentAnimation < animations.size());
	Animation &animation = animations[currentAnimation];

	for (Bone *bone : bones)
	{
		glm::mat4 translation = animation.getTranslation(bone);
		glm::mat4 rotation = glm::toMat4(animation.getRotation(bone));
		glm::mat4 scaling = animation.getScale(bone);
		bone->setFlatMatrix(translation * rotation * scaling);
	}

	Model::update(deltaTime);
}

void AnimatedModel::render(/*SkinnedForwardShader *skinnedForwardShader*/)
{
	float *data;
	if (!Error::checkMemory(data = new float[MAX_BONES * 16])) return;

	for (int i = 0; i < MAX_BONES; ++i)
	{
		glm::mat4 totalMatrix = glm::mat4();

		if (i < bones.size())
			totalMatrix = bones[i]->getFinalMatrix();

		for (int j = 0; j < 4; ++j)
		{
			for (int k = 0; k < 4; ++k)
			{
				const unsigned int index = i * 16 + j * 4 + k;
				data[index] = totalMatrix[j][k];
			}
		}
	}

	//assert(skinnedForwardShader);
	//skinnedForwardShader->setBoneMatrixUniforms(data, MAX_BONES);
	delete data;

	Model::render();
}