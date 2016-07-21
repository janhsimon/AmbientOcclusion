#pragma once

#include <assimp\scene.h>

class Vertex
{
private:
	float position[3];
	float normal[3];
	float color[4];
	float boneWeight[4];
	unsigned int boneID[4];

public:
	Vertex(const aiVector3D &position, const aiVector3D &normal, const aiColor4D &color);
	
	inline float getBoneWeight(unsigned int index) const { assert(index < 4); return boneWeight[index]; }
	inline void setBoneInfo(unsigned int index, unsigned int id, float weight) { assert(index < 4); boneID[index] = id; boneWeight[index] = weight; }
};