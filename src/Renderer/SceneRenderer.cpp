#include <random>
#include <gtc\type_ptr.hpp>
#include <gtx\transform.hpp>
#include <gtc\random.hpp>

#include "SceneRenderer.hpp"
#include "..\Error.hpp"

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
{
	return a + f * (b - a);
}

SceneRenderer::~SceneRenderer()
{
	deleteShaderPrograms();
	deleteShaders();

	if (gBuffer) delete gBuffer;
	if (ssaoBuffer) delete ssaoBuffer;
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
	if (!Error::checkMemory(blurVertexShader = new Shader())) return false;
	if (!blurVertexShader->load("Shaders\\Blur.vs.glsl", GL_VERTEX_SHADER)) return false;
	if (!Error::checkMemory(standardVertexShader = new Shader())) return false;
	if (!standardVertexShader->load("Shaders\\Standard.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load fragment shaders for geometry pass
	if (!Error::checkMemory(geometryFragmentShader = new Shader())) return false;
	if (!geometryFragmentShader->load("Shaders\\Geometry.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	// load fragment shaders for lighting passes
	if (!Error::checkMemory(directionalLightFragmentShader = new Shader())) return false;
	if (!directionalLightFragmentShader->load("Shaders\\DirectionalLight.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	// load fragment shaders for other post-processing passes
	if (!Error::checkMemory(ambientOcclusionFragmentShader = new Shader())) return false;
	if (!ambientOcclusionFragmentShader->load("Shaders\\AmbientOcclusion.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	if (!Error::checkMemory(blurFragmentShader = new Shader())) return false;
	if (!blurFragmentShader->load("Shaders\\Blur.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	if (!Error::checkMemory(standardFragmentShader = new Shader())) return false;
	if (!standardFragmentShader->load("Shaders\\Standard.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	return true;
}

bool SceneRenderer::loadShaderPrograms(const Camera *camera)
{
	assert(camera);
	assert(ssaoBuffer);
	assert(standardVertexShader);
	assert(geometryVertexShader);
	assert(skinnedGeometryVertexShader);
	assert(directionalLightVertexShader);
	assert(ambientOcclusionVertexShader);
	assert(blurVertexShader);
	assert(standardFragmentShader);
	assert(geometryFragmentShader);
	assert(directionalLightFragmentShader);
	assert(ambientOcclusionFragmentShader);
	assert(blurFragmentShader);

	if (!Error::checkMemory(geometryShaderProgram = new ShaderProgram())) return false;
	if (!geometryShaderProgram->load(geometryVertexShader, geometryFragmentShader)) return false;
	glUseProgram(geometryShaderProgram->getHandle());
	glUniformMatrix4fv(geometryShaderProgram->getUniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
	glUniform2f(geometryShaderProgram->getUniform("cameraNearFar"), camera->getNear(), camera->getFar());

	if (!Error::checkMemory(skinnedGeometryShaderProgram = new ShaderProgram())) return false;
	if (!skinnedGeometryShaderProgram->load(skinnedGeometryVertexShader, geometryFragmentShader)) return false;

	if (!Error::checkMemory(directionalLightShaderProgram = new ShaderProgram())) return false;
	if (!directionalLightShaderProgram->load(directionalLightVertexShader, directionalLightFragmentShader)) return false;
	glUseProgram(directionalLightShaderProgram->getHandle());
	glUniform2f(directionalLightShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);
	glUniform1i(directionalLightShaderProgram->getUniform("inNormal"), 1);
	glUniform1i(directionalLightShaderProgram->getUniform("inColor"), 2);

	if (!Error::checkMemory(ambientOcclusionShaderProgram = new ShaderProgram())) return false;
	if (!ambientOcclusionShaderProgram->load(ambientOcclusionVertexShader, ambientOcclusionFragmentShader)) return false;
	glUseProgram(ambientOcclusionShaderProgram->getHandle());
	glUniform2f(ambientOcclusionShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);
	glUniformMatrix4fv(ambientOcclusionShaderProgram->getUniform("projectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix()));
	glUniform1i(ambientOcclusionShaderProgram->getUniform("inPosition"), 0);
	glUniform1i(ambientOcclusionShaderProgram->getUniform("inNormal"), 1);
	glUniform1i(ambientOcclusionShaderProgram->getUniform("inNoise"), 3);

	// sample kernel
	std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	glm::vec3 samples[64];
	for (GLuint i = 0; i < 64; ++i)
	{
		glm::vec3 sample(randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator) * 2.0f - 1.0f, randomFloats(generator));
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		GLfloat scale = GLfloat(i) / 64.0f;

		// scale samples so that they are more aligned to the center of the kernel
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		samples[i] = sample;
	}
	glUniform3fv(ambientOcclusionShaderProgram->getUniform("samples"), 64, glm::value_ptr(samples[0]));

	if (!Error::checkMemory(blurShaderProgram = new ShaderProgram())) return false;
	if (!blurShaderProgram->load(blurVertexShader, blurFragmentShader)) return false;
	glUseProgram(blurShaderProgram->getHandle());
	glUniform2f(blurShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);
	glUniform1i(blurShaderProgram->getUniform("inAmbientOcclusion"), 0);

	if (!Error::checkMemory(standardShaderProgram = new ShaderProgram())) return false;
	if (!standardShaderProgram->load(standardVertexShader, standardFragmentShader)) return false;
	glUseProgram(standardShaderProgram->getHandle());
	glUniform2f(standardShaderProgram->getUniform("screenSize"), 1280.0f, 720.0f);
	glUniform1i(standardShaderProgram->getUniform("inTexture"), 0);

	return true;
}

void SceneRenderer::deleteShaders()
{
	// delete vertex shaders
	if (standardVertexShader) delete standardVertexShader;
	if (geometryVertexShader) delete geometryVertexShader;
	if (skinnedGeometryVertexShader) delete skinnedGeometryVertexShader;
	if (directionalLightVertexShader) delete directionalLightVertexShader;
	if (ambientOcclusionVertexShader) delete ambientOcclusionVertexShader;
	if (blurVertexShader) delete blurVertexShader;

	// delete fragment shaders
	if (standardFragmentShader) delete standardFragmentShader;
	if (geometryFragmentShader) delete geometryFragmentShader;
	if (directionalLightFragmentShader) delete directionalLightFragmentShader;
	if (ambientOcclusionFragmentShader) delete ambientOcclusionFragmentShader;
	if (blurFragmentShader) delete blurFragmentShader;
}

void SceneRenderer::deleteShaderPrograms()
{
	if (standardShaderProgram) delete standardShaderProgram;
	if (geometryShaderProgram) delete geometryShaderProgram;
	if (skinnedGeometryShaderProgram) delete skinnedGeometryShaderProgram;
	if (directionalLightShaderProgram) delete directionalLightShaderProgram;
	if (ambientOcclusionShaderProgram) delete ambientOcclusionShaderProgram;
	if (blurShaderProgram) delete blurShaderProgram;
}

bool SceneRenderer::load(const ModelManager *modelManager, const LightManager *lightManager, const Camera *camera)
{
	assert(modelManager);
	this->modelManager = modelManager;

	assert(lightManager);
	this->lightManager = lightManager;

	if (!Error::checkMemory(gBuffer = new GBuffer())) return false;
	if (!gBuffer->load(1280, 720)) return false;

	if (!Error::checkMemory(ssaoBuffer = new SSAOBuffer())) return false;
	if (!ssaoBuffer->load(1280, 720)) return false;

	assert(camera);
	if (!loadShaders()) return false;
	if (!loadShaderPrograms(camera)) return false;

	return true;
}

void SceneRenderer::renderGeometryPass(const Camera *camera)
{
	// render to the geometry buffer
	assert(gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->getFBO());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// use the geometry shader program
	assert(geometryShaderProgram);
	glUseProgram(geometryShaderProgram->getHandle());

	// set the camera view matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(geometryShaderProgram->getUniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));

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
	// render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	// blend lights additively for proper accumulation
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// use the directional light shader program
	assert(directionalLightShaderProgram);
	glUseProgram(directionalLightShaderProgram->getHandle());

	// set the camera view matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(directionalLightShaderProgram->getUniform("viewMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getViewMatrix()));

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

void SceneRenderer::renderAmbientOcclusionPass()
{
	// render to the first ssao buffer
	assert(ssaoBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer->getFBO(0));
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, ssaoBuffer->getNoiseTexture());

	// use the ambient occlusion shader program
	assert(ambientOcclusionShaderProgram);
	glUseProgram(ambientOcclusionShaderProgram->getHandle());

	// get the directional light geometry
	assert(modelManager);
	Model *directionalLightGeometry = modelManager->getDirectionalLightGeometry();
	assert(directionalLightGeometry);
	directionalLightGeometry->render();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneRenderer::renderBlurPass()
{
	// render to the second ssao buffer
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer->getFBO(1));
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ssaoBuffer->getTexture(0));

	// use the blur shader program
	assert(blurShaderProgram);
	glUseProgram(blurShaderProgram->getHandle());

	assert(modelManager);
	Model *directionalLightGeometry = modelManager->getDirectionalLightGeometry();
	assert(directionalLightGeometry);
	directionalLightGeometry->render();

	// render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// now multiply the ambient occlusion in
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	// use the standard shader program
	assert(standardShaderProgram);
	glUseProgram(standardShaderProgram->getHandle());

	glBindTexture(GL_TEXTURE_2D, ssaoBuffer->getTexture(1));

	directionalLightGeometry->render();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void SceneRenderer::render(const Camera *camera)
{
	assert(camera);
	renderGeometryPass(camera);

	// bind the geometry buffer textures
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->getTexture(i));
	}

	renderAmbientOcclusionPass();
	renderLightingPass(camera);

	// unbind the geometry buffer textures
	for (unsigned int i = 0; i < 3; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	renderBlurPass();
}