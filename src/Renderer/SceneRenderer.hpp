#pragma once

#include "GBuffer.hpp"
#include "..\Camera.hpp"
#include "..\Model\ModelManager.hpp"
#include "..\Shader\Shader.hpp"
#include "..\Shader\ShaderProgram.hpp"

class SceneRenderer
{
private:
	const ModelManager *modelManager;
	Shader *geometryVertexShader, *skinnedGeometryVertexShader, *lightingVertexShader;
	Shader *geometryFragmentShader, *lightingFragmentShader;
	ShaderProgram *geometryShaderProgram, *skinnedGeometryShaderProgram, *lightingShaderProgram;
	GBuffer *gBuffer;

	bool loadShaders();
	bool loadShaderPrograms();
	void deleteShaders();
	void deleteShaderPrograms();
	void renderGeometryPass(const Camera *camera);
	void renderLightingPass(const Camera *camera);

public:
	~SceneRenderer();

	bool load(const ModelManager *modelManager);
	void render(const Camera *camera);
};