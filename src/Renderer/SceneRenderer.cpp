#include <gtc\type_ptr.hpp>
#include <gtx\transform.hpp>

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
	// load vertex shaders
	if (!Error::checkMemory(geometryVertexShader = new Shader())) return false;
	if (!geometryVertexShader->load("Shaders\\Geometry.vs.glsl", GL_VERTEX_SHADER)) return false;
	if (!Error::checkMemory(skinnedGeometryVertexShader = new Shader())) return false;
	if (!skinnedGeometryVertexShader->load("Shaders\\SkinnedGeometry.vs.glsl", GL_VERTEX_SHADER)) return false;
	if (!Error::checkMemory(lightingVertexShader = new Shader())) return false;
	if (!lightingVertexShader->load("Shaders\\Lighting.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load fragment shaders
	if (!Error::checkMemory(geometryFragmentShader = new Shader())) return false;
	if (!geometryFragmentShader->load("Shaders\\Geometry.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	if (!Error::checkMemory(directionalLightFragmentShader = new Shader())) return false;
	if (!directionalLightFragmentShader->load("Shaders\\DirectionalLight.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	return true;
}

bool SceneRenderer::loadShaderPrograms()
{
	assert(geometryVertexShader);
	assert(skinnedGeometryVertexShader);
	assert(lightingVertexShader);
	assert(geometryFragmentShader);
	assert(directionalLightFragmentShader);

	if (!Error::checkMemory(geometryShaderProgram = new ShaderProgram())) return false;
	if (!geometryShaderProgram->load(geometryVertexShader, geometryFragmentShader)) return false;
	if (!geometryShaderProgram->registerUniform("worldMatrix")) return false;
	if (!geometryShaderProgram->registerUniform("viewProjectionMatrix")) return false;

	if (!Error::checkMemory(skinnedGeometryShaderProgram = new ShaderProgram())) return false;
	if (!skinnedGeometryShaderProgram->load(skinnedGeometryVertexShader, geometryFragmentShader)) return false;
	if (!skinnedGeometryShaderProgram->registerUniform("worldMatrix")) return false;
	if (!skinnedGeometryShaderProgram->registerUniform("viewProjectionMatrix")) return false;

	if (!Error::checkMemory(directionalLightShaderProgram = new ShaderProgram())) return false;
	if (!directionalLightShaderProgram->load(lightingVertexShader, directionalLightFragmentShader)) return false;
	if (!directionalLightShaderProgram->registerUniform("inGBufferMRT0")) return false;
	if (!directionalLightShaderProgram->registerUniform("inGBufferMRT1")) return false;
	if (!directionalLightShaderProgram->registerUniform("screenSize")) return false;
	if (!directionalLightShaderProgram->registerUniform("lightDirection")) return false;
	if (!directionalLightShaderProgram->registerUniform("lightColor")) return false;
	glUseProgram(directionalLightShaderProgram->getHandle());
	glUniform1i(directionalLightShaderProgram->getUniform("inGBufferMRT0"), 0);
	glUniform1i(directionalLightShaderProgram->getUniform("inGBufferMRT1"), 1);
	glUniform2f(directionalLightShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);

	return true;
}

void SceneRenderer::deleteShaders()
{
	// delete vertex shaders
	if (geometryVertexShader) delete geometryVertexShader;
	if (skinnedGeometryVertexShader) delete skinnedGeometryVertexShader;
	if (lightingVertexShader) delete lightingVertexShader;

	// delete fragment shaders
	if (geometryFragmentShader) delete geometryFragmentShader;
	if (directionalLightFragmentShader) delete directionalLightFragmentShader;
}

void SceneRenderer::deleteShaderPrograms()
{
	if (geometryShaderProgram) delete geometryShaderProgram;
	if (skinnedGeometryShaderProgram) delete skinnedGeometryShaderProgram;
	if (directionalLightShaderProgram) delete directionalLightShaderProgram;
}

bool SceneRenderer::load(const ModelManager *modelManager, const LightManager *lightManager)
{
	assert(modelManager);
	this->modelManager = modelManager;

	assert(lightManager);
	this->lightManager = lightManager;

	// load shaders and shader programs
	if (!loadShaders()) return false;
	if (!loadShaderPrograms()) return false;

	// load geometry buffer
	if (!Error::checkMemory(gBuffer = new GBuffer())) return false;
	if (!gBuffer->load(1280, 720)) return false;

	return true;
}

void SceneRenderer::renderGeometryPass(const Camera *camera)
{
	assert(gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->getFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// use the geometry shader program
	assert(geometryShaderProgram);
	glUseProgram(geometryShaderProgram->getHandle());

	// set the camera projection matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(geometryShaderProgram->getUniform("viewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix() * camera->getViewMatrix()));

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

void SceneRenderer::renderLightingPass(const Camera *camera)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);

	// bind the geometry buffer textures
	for (unsigned int i = 0; i < 2; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(i));
	}

	// use the lighting shader program
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
		glUniform3f(directionalLightShaderProgram->getUniform("lightDirection"), directionalLight->getForward().x, directionalLight->getForward().y, directionalLight->getForward().z);
		glUniform3f(directionalLightShaderProgram->getUniform("lightColor"), directionalLight->getColor().r, directionalLight->getColor().g, directionalLight->getColor().b);
		directionalLightGeometry->render();
	}

	glDisable(GL_BLEND);

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

void SceneRenderer::render(const Camera *camera)
{
	assert(camera);
	renderGeometryPass(camera);
	renderLightingPass(camera);
}