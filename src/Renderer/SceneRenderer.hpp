#pragma once

#include "GBuffer.hpp"
#include "..\Camera.hpp"
#include "..\Light\LightManager.hpp"
#include "..\Model\ModelManager.hpp"
#include "..\Shader\Shader.hpp"
#include "..\Shader\ShaderProgram.hpp"

class SceneRenderer
{
private:
	const ModelManager *modelManager;
	const LightManager *lightManager;
	Shader *geometryVertexShader, *skinnedGeometryVertexShader, *directionalLightVertexShader, *ambientOcclusionVertexShader;
	Shader *geometryFragmentShader, *directionalLightFragmentShader, *ambientOcclusionFragmentShader;
	ShaderProgram *geometryShaderProgram, *skinnedGeometryShaderProgram, *directionalLightShaderProgram, *ambientOcclusionShaderProgram;
	GBuffer *gBuffer;

	bool loadShaders();
	bool loadShaderPrograms();
	void deleteShaders();
	void deleteShaderPrograms();
	void renderGeometryPass(const Camera *camera);
	void renderLightingPass();
	void renderAmbientOcclusionPass(const Camera *camera);

public:
	~SceneRenderer();

	bool load(const ModelManager *modelManager, const LightManager *lightManager);
	void render(const Camera *camera);
};