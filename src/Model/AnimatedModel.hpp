#pragma once

#include "Animation.hpp"
#include "Model.hpp"
//#include "..\Shader\SkinnedForwardShader.hpp"

class AnimatedModel : public Model
{
private:
	static const unsigned int MAX_BONES;

	std::vector<Bone*> bones;
	std::vector<Animation> animations;
	unsigned int currentAnimation;
	glm::mat4 *finalBoneMatrices;

	Bone *getBoneFromName(const std::string &name);
	void loadBones(const aiScene *model, const aiMesh *mesh, unsigned int startingVertex);
	void loadAnimations(const aiScene *model);

public:
	AnimatedModel(const glm::vec3 &position = glm::vec3(0.0f));
	~AnimatedModel();

	bool load(const std::string &filename);
	void changeAnimation();
	void update(float deltaTime);
	void render(/*SkinnedForwardShader *skinnedForwardShader*/);
};