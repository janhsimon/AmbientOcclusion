#include "Vertex.hpp"

Vertex::Vertex(const aiVector3D &position, const aiVector3D &normal, const aiColor4D &color)
{
	for (int i = 0; i < 4; ++i)
	{
		if (i < 3)
		{
			this->position[i] = position[i];
			this->normal[i] = normal[i];
		}

		this->color[i] = color[i];
		this->boneWeight[i] = 0.0f;
		this->boneID[i] = 0;
	}
}