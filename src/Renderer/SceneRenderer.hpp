#pragma once

#include "GBuffer.hpp"
#include "SSAOBuffer.hpp"
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
	Shader *standardVertexShader, *geometryVertexShader, *skinnedGeometryVertexShader, *directionalLightVertexShader, *ambientOcclusionVertexShader, *blurVertexShader;
	Shader *standardFragmentShader, *geometryFragmentShader, *directionalLightFragmentShader, *ambientOcclusionFragmentShader, *blurFragmentShader;
	ShaderProgram *standardShaderProgram, *geometryShaderProgram, *skinnedGeometryShaderProgram, *directionalLightShaderProgram, *ambientOcclusionShaderProgram, *blurShaderProgram;
	GBuffer *gBuffer;
	SSAOBuffer *ssaoBuffer;

	bool loadShaders();
	bool loadShaderPrograms(const Camera *camera);
	void deleteShaders();
	void deleteShaderPrograms();
	void renderGeometryPass(const Camera *camera);
	void renderLightingPass(const Camera *camera);
	void renderAmbientOcclusionPass();
	void renderBlurPass();

public:
	~SceneRenderer();

	bool load(const ModelManager *modelManager, const LightManager *lightManager, const Camera *camera);
	void render(const Camera *camera);
};