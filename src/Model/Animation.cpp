#include "Animation.hpp"
#include "..\Error.hpp"

Animation::Animation(float speed, bool loop)
{
	this->speed = speed;
	this->loop = loop;

	timer = 0.0f;
}

void Animation::update(float deltaTime)
{
	timer += deltaTime * 0.01f;

	if (loop)
	{
		while (timer >= 1.0f)
			timer -= 1.0f;
	}
}

void Animation::addTranslationKey(Bone *bone, const glm::mat4 &translationKey)
{
	translationKeysMap[bone].push_back(translationKey);
}

void Animation::addRotationKey(Bone *bone, const glm::quat &rotationKey)
{
	rotationKeysMap[bone].push_back(rotationKey);
}

void Animation::addScaleKey(Bone *bone, const glm::mat4 &scaleKey)
{
	scaleKeysMap[bone].push_back(scaleKey);
}

glm::mat4 Animation::getTranslation(Bone *bone)
{
	if (translationKeysMap.find(bone) != translationKeysMap.end())
	{
		Error::report("Bone \"" + bone->getName() + "\" required but not found.");
		return glm::mat4();
	}

	std::vector<glm::mat4> translationMatrices = translationKeysMap.at(bone);

	unsigned int keyIndex = unsigned int(timer * translationMatrices.size());

	if (keyIndex >= translationMatrices.size())
		keyIndex = (unsigned int)translationMatrices.size() - 1;

	return translationMatrices.at(keyIndex);
}

glm::quat Animation::getRotation(Bone *bone)
{
	if (rotationKeysMap.find(bone) != rotationKeysMap.end())
	{
		Error::report("Bone \"" + bone->getName() + "\" required but not found.");
		return glm::quat();
	}

	std::vector<glm::quat> rotationQuaternions = rotationKeysMap.at(bone);

	unsigned int keyIndex = unsigned int(timer * rotationQuaternions.size());

	if (keyIndex >= rotationQuaternions.size())
		keyIndex = (unsigned int)rotationQuaternions.size() - 1;

	return rotationQuaternions.at(keyIndex);
}

glm::mat4 Animation::getScale(Bone *bone)
{
	if (scaleKeysMap.find(bone) != scaleKeysMap.end())
	{
		Error::report("Bone \"" + bone->getName() + "\" required but not found.");
		return glm::mat4();
	}

	std::vector<glm::mat4> scaleMatrices = scaleKeysMap.at(bone);

	unsigned int keyIndex = unsigned int(timer * scaleMatrices.size());

	if (keyIndex >= scaleMatrices.size())
		keyIndex = (unsigned int)scaleMatrices.size() - 1;

	return scaleMatrices.at(keyIndex);
}