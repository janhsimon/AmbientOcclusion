#pragma once

#include "..\Camera.hpp"
#include "..\Model\ModelManager.hpp"
#include "..\Shader\Shader.hpp"
#include "..\Shader\ShaderProgram.hpp"

class SceneRenderer
{
private:
	const ModelManager *modelManager;
	Shader *standardTransformVertexShader, *skinnedTransformVertexShader, *noTransformVertexShader;
	Shader *forwardLitFragmentShader, *forwardUnlitFragmentShader, *gBufferFillFragmentShader, *lightingFragmentShader;
	ShaderProgram *forwardStandardShaderProgram, *forwardSkinnedShaderProgram, *geometryPassStandardShaderProgram, *geometryPassSkinnedShaderProgram, *lightingPassShaderProgram;

	bool loadShaders();
	bool loadShaderPrograms();
	void renderForwardPass(const Camera *camera);
	void renderGeometryPass(const Camera *camera);
	void renderLightingPass(const Camera *camera);

public:
	~SceneRenderer();

	bool load(const ModelManager *modelManager);
	void render(const Camera *camera);
};