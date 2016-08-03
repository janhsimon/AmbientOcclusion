#pragma once

#include <unordered_map>
#include <vector>
#include <gtx\quaternion.hpp>

#include "Bone.hpp"

class Animation
{
private:
	float timer;
	float speed;
	bool loop;
	std::unordered_map<Bone*, std::vector<glm::mat4>> translationKeysMap;
	std::unordered_map<Bone*, std::vector<glm::quat>> rotationKeysMap;
	std::unordered_map<Bone*, std::vector<glm::mat4>> scaleKeysMap;

public:
	Animation(float speed = 0.001f, bool loop = true);

	void update(float deltaTime);

	void addTranslationKey(Bone *bone, const glm::mat4 &translationKey);
	void addRotationKey(Bone *bone, const glm::quat &rotationKey);
	void addScaleKey(Bone *bone, const glm::mat4 &scaleKey);

	glm::mat4 getTranslation(Bone *bone);
	glm::quat getRotation(Bone *bone);
	glm::mat4 getScale(Bone *bone);
};