#pragma once

#include "Model.hpp"
#include "..\Shader\SkinnedForwardShader.hpp"

class AnimatedModel : public Model
{
private:
	static const unsigned int MAX_BONES;

	std::vector<Bone> bones;
	aiNode *rootNode;
	glm::mat4 *finalBoneMatrices;
	aiAnimation **animations;
	float animationTimer;
	float animationSpeed;
	unsigned int currentAnimation;
	unsigned int currentKey;
	unsigned int numAnimations;

	void loadBones(const aiMesh *mesh, unsigned int startingVertex);

public:
	AnimatedModel(const glm::vec3 &position = glm::vec3(0.0f));
	~AnimatedModel();

	bool load(const std::string &filename);
	void changeAnimation();
	void update(float delta);
	void render(SkinnedForwardShader *skinnedForwardShader);
};