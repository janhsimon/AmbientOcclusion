#include <cassert>

#include "ShaderManager.hpp"

ShaderManager::~ShaderManager()
{
	for (auto shader : shaders)
		delete shader.second;
}

bool ShaderManager::loadShader(const std::string &filename, GLenum type)
{
	Shader *shader = new Shader();

	if (!shader->load(filename, type))
		return false;

	shaders[filename] = shader;
	return true;
}

Shader *ShaderManager::getShader(const std::string &name)
{
	std::unordered_map<std::string, Shader*>::const_iterator shader = shaders.find(name);
	assert(shader != shaders.end());
	return shader->second;
}