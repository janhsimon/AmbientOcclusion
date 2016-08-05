#include <gtc\type_ptr.hpp>
#include <gtx\transform.hpp>
#include <gtc\random.hpp>

#include "SceneRenderer.hpp"
#include "..\Error.hpp"

SceneRenderer::~SceneRenderer()
{
	deleteShaderPrograms();
	deleteShaders();

	if (gBuffer) delete gBuffer;
}

bool SceneRenderer::loadShaders()
{
	// load vertex shaders for geometry pass
	if (!Error::checkMemory(geometryVertexShader = new Shader())) return false;
	if (!geometryVertexShader->load("Shaders\\Geometry.vs.glsl", GL_VERTEX_SHADER)) return false;
	if (!Error::checkMemory(skinnedGeometryVertexShader = new Shader())) return false;
	if (!skinnedGeometryVertexShader->load("Shaders\\SkinnedGeometry.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load vertex shaders for lighting passes
	if (!Error::checkMemory(directionalLightVertexShader = new Shader())) return false;
	if (!directionalLightVertexShader->load("Shaders\\DirectionalLight.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load vertex shaders for other post-processing passes
	if (!Error::checkMemory(ambientOcclusionVertexShader = new Shader())) return false;
	if (!ambientOcclusionVertexShader->load("Shaders\\AmbientOcclusion.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load fragment shaders for geometry pass
	if (!Error::checkMemory(geometryFragmentShader = new Shader())) return false;
	if (!geometryFragmentShader->load("Shaders\\Geometry.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	// load fragment shaders for lighting passes
	if (!Error::checkMemory(directionalLightFragmentShader = new Shader())) return false;
	if (!directionalLightFragmentShader->load("Shaders\\DirectionalLight.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	// load fragment shaders for other post-processing passes
	if (!Error::checkMemory(ambientOcclusionFragmentShader = new Shader())) return false;
	if (!ambientOcclusionFragmentShader->load("Shaders\\AmbientOcclusion.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	return true;
}

bool SceneRenderer::loadShaderPrograms()
{
	assert(geometryVertexShader);
	assert(skinnedGeometryVertexShader);
	assert(directionalLightVertexShader);
	assert(ambientOcclusionVertexShader);
	assert(geometryFragmentShader);
	assert(directionalLightFragmentShader);
	assert(ambientOcclusionFragmentShader);

	if (!Error::checkMemory(geometryShaderProgram = new ShaderProgram())) return false;
	if (!geometryShaderProgram->load(geometryVertexShader, geometryFragmentShader)) return false;

	if (!Error::checkMemory(skinnedGeometryShaderProgram = new ShaderProgram())) return false;
	if (!skinnedGeometryShaderProgram->load(skinnedGeometryVertexShader, geometryFragmentShader)) return false;

	if (!Error::checkMemory(directionalLightShaderProgram = new ShaderProgram())) return false;
	if (!directionalLightShaderProgram->load(directionalLightVertexShader, directionalLightFragmentShader)) return false;
	glUseProgram(directionalLightShaderProgram->getHandle());
	glUniform1i(directionalLightShaderProgram->getUniform("inGBufferMRT0"), 0);
	glUniform1i(directionalLightShaderProgram->getUniform("inGBufferMRT1"), 1);
	glUniform2f(directionalLightShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);

	if (!Error::checkMemory(ambientOcclusionShaderProgram = new ShaderProgram())) return false;
	if (!ambientOcclusionShaderProgram->load(ambientOcclusionVertexShader, ambientOcclusionFragmentShader)) return false;
	glUseProgram(ambientOcclusionShaderProgram->getHandle());
	glUniform1i(ambientOcclusionShaderProgram->getUniform("inGBufferMRT0"), 0);
	glUniform1i(ambientOcclusionShaderProgram->getUniform("inGBufferMRT2"), 2);
	glUniform2f(ambientOcclusionShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);

	glm::vec3 randomBallSamples[32];
	for (unsigned int i = 0; i < 32; ++i)
		randomBallSamples[i] = glm::ballRand(1.0f);
	glUniform3fv(ambientOcclusionShaderProgram->getUniform("randomBallSamples"), 32, glm::value_ptr(randomBallSamples[0]));

	return true;
}

void SceneRenderer::deleteShaders()
{
	// delete vertex shaders
	if (geometryVertexShader) delete geometryVertexShader;
	if (skinnedGeometryVertexShader) delete skinnedGeometryVertexShader;
	if (directionalLightVertexShader) delete directionalLightVertexShader;
	if (ambientOcclusionVertexShader) delete ambientOcclusionVertexShader;

	// delete fragment shaders
	if (geometryFragmentShader) delete geometryFragmentShader;
	if (directionalLightFragmentShader) delete directionalLightFragmentShader;
	if (ambientOcclusionFragmentShader) delete ambientOcclusionFragmentShader;
}

void SceneRenderer::deleteShaderPrograms()
{
	if (geometryShaderProgram) delete geometryShaderProgram;
	if (skinnedGeometryShaderProgram) delete skinnedGeometryShaderProgram;
	if (directionalLightShaderProgram) delete directionalLightShaderProgram;
	if (ambientOcclusionShaderProgram) delete ambientOcclusionShaderProgram;
}

bool SceneRenderer::load(const ModelManager *modelManager, const LightManager *lightManager)
{
	assert(modelManager);
	this->modelManager = modelManager;

	assert(lightManager);
	this->lightManager = lightManager;

	if (!loadShaders()) return false;
	if (!loadShaderPrograms()) return false;

	if (!Error::checkMemory(gBuffer = new GBuffer())) return false;
	if (!gBuffer->load(1280, 720)) return false;

	return true;
}

void SceneRenderer::renderGeometryPass(const Camera *camera)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// use the geometry shader program
	assert(geometryShaderProgram);
	glUseProgram(geometryShaderProgram->getHandle());

	// set the camera view and projection matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(geometryShaderProgram->getUniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
	glUniformMatrix4fv(geometryShaderProgram->getUniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
	glUniform3fv(geometryShaderProgram->getUniform("eyePosition"), 1, glm::value_ptr(camera->getPosition()));

	assert(modelManager);
	for (unsigned int modelIndex = 0; modelIndex < modelManager->getNumModels(); ++modelIndex)
	// loop through each model
	{
		// set the world matrix and render the model
		Model *model = modelManager->getModelAt(modelIndex);
		assert(model);
		glUniformMatrix4fv(geometryShaderProgram->getUniform("worldMatrix"), 1, GL_FALSE, glm::value_ptr(model->getWorldMatrix()));
		model->render();
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
}

void SceneRenderer::renderLightingPass()
{
	// use the directional light shader program
	assert(directionalLightShaderProgram);
	glUseProgram(directionalLightShaderProgram->getHandle());

	// get the directional light geometry
	assert(modelManager);
	Model *directionalLightGeometry = modelManager->getDirectionalLightGeometry();
	assert(directionalLightGeometry);

	assert(lightManager);
	for (unsigned int directionalLightIndex = 0; directionalLightIndex < lightManager->getNumDirectionalLights(); ++directionalLightIndex)
	// loop through each directional light
	{
		DirectionalLight *directionalLight = lightManager->getDirectionalLightAt(directionalLightIndex);
		assert(directionalLight);
		glUniform3fv(directionalLightShaderProgram->getUniform("lightDirection"), 1, glm::value_ptr(directionalLight->getForward()));
		glUniform3fv(directionalLightShaderProgram->getUniform("lightColor"), 1, glm::value_ptr(directionalLight->getColor()));
		directionalLightGeometry->render();
	}

	/*
	glFrontFace(GL_CCW);

	Model *testSphereModel = modelManager->getTestSphereModel();
	assert(testSphereModel);
	glUniformMatrix4fv(forwardStandardUnlitShaderProgram->getUniform("worldMatrix"), 1, GL_FALSE, glm::value_ptr(testSphereModel->getWorldMatrix()));
	testSphereModel->render();

	assert(modelManager);
	Model *pointLightVolumeModel = modelManager->getPointLightVolumeModel();
	assert(pointLightVolumeModel);
	pointLightVolumeModel->setPosition(testSphereModel->getPosition());
	pointLightVolumeModel->setScale(testSphereModel->getScale());
	pointLightVolumeModel->update(0.0f);

	glUniformMatrix4fv(lightingPassShaderProgram->getUniform("worldMatrix"), 1, GL_FALSE, glm::value_ptr(pointLightVolumeModel->getWorldMatrix()));
	pointLightVolumeModel->render();

	glFrontFace(GL_CW);
	*/
}

void SceneRenderer::renderAmbientOcclusionPass(const Camera *camera)
{
	// use the ambient occlusion shader program
	assert(ambientOcclusionShaderProgram);
	glUseProgram(ambientOcclusionShaderProgram->getHandle());

	// set the camera view and projection matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(ambientOcclusionShaderProgram->getUniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));
	glUniformMatrix4fv(ambientOcclusionShaderProgram->getUniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));

	// get the directional light geometry
	assert(modelManager);
	Model *directionalLightGeometry = modelManager->getDirectionalLightGeometry();
	assert(directionalLightGeometry);

	glUniform3fv(ambientOcclusionShaderProgram->getUniform("eyePosition"), 1, glm::value_ptr(camera->getPosition()));
	directionalLightGeometry->render();
}

void SceneRenderer::render(const Camera *camera)
{
	// render to the geometry buffer
	assert(gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->getFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	assert(camera);
	renderGeometryPass(camera);

	// render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind the geometry buffer textures
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(i));
	}

	// blend lights additively for proper accumulation
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	renderLightingPass();

	// now multiply the ambient occlusion in
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	renderAmbientOcclusionPass(camera);

	glDisable(GL_BLEND);

	// unbind the geometry buffer textures
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}