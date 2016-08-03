#include <gtc\type_ptr.hpp>
#include <gtx\transform.hpp>

#include "SceneRenderer.hpp"

SceneRenderer::~SceneRenderer()
{
	// delete shader programs
	if (forwardStandardShaderProgram) delete forwardStandardShaderProgram;
	if (forwardSkinnedShaderProgram) delete forwardSkinnedShaderProgram;
	//if (geometryPassStandardShaderProgram) delete geometryPassStandardShaderProgram;
	//if (geometryPassSkinnedShaderProgram) delete geometryPassSkinnedShaderProgram;
	if (lightingPassShaderProgram) delete lightingPassShaderProgram;

	// delete vertex shaders
	if (standardTransformVertexShader) delete standardTransformVertexShader;
	if (skinnedTransformVertexShader) delete skinnedTransformVertexShader;
	if (noTransformVertexShader) delete noTransformVertexShader;
	
	// delete fragment shaders
	if (forwardLitFragmentShader) delete forwardLitFragmentShader;
	if (forwardUnlitFragmentShader) delete forwardUnlitFragmentShader;
	if (gBufferFillFragmentShader) delete gBufferFillFragmentShader;
	if (lightingFragmentShader) delete lightingFragmentShader;
}

bool SceneRenderer::loadShaders()
{
	// load vertex shaders
	standardTransformVertexShader = new Shader();
	if (!standardTransformVertexShader->load("Shaders\\StandardTransform.vs.glsl", GL_VERTEX_SHADER)) return false;
	skinnedTransformVertexShader = new Shader();
	if (!skinnedTransformVertexShader->load("Shaders\\SkinnedTransform.vs.glsl", GL_VERTEX_SHADER)) return false;
	noTransformVertexShader = new Shader();
	if (!noTransformVertexShader->load("Shaders\\NoTransform.vs.glsl", GL_VERTEX_SHADER)) return false;

	// load fragment shaders
	forwardLitFragmentShader = new Shader();
	if (!forwardLitFragmentShader->load("Shaders\\Lit.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	forwardUnlitFragmentShader = new Shader();
	if (!forwardUnlitFragmentShader->load("Shaders\\Unlit.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	gBufferFillFragmentShader = new Shader();
	if (!gBufferFillFragmentShader->load("Shaders\\GBufferFill.fs.glsl", GL_FRAGMENT_SHADER)) return false;
	lightingFragmentShader = new Shader();
	if (!lightingFragmentShader->load("Shaders\\Lighting.fs.glsl", GL_FRAGMENT_SHADER)) return false;

	return true;
}

bool SceneRenderer::loadShaderPrograms()
{
	assert(standardTransformVertexShader);
	assert(skinnedTransformVertexShader);
	assert(noTransformVertexShader);
	assert(forwardLitFragmentShader);
	assert(forwardUnlitFragmentShader);
	assert(gBufferFillFragmentShader);
	assert(lightingFragmentShader);

	forwardStandardShaderProgram = new ShaderProgram();
	if (!forwardStandardShaderProgram->load(standardTransformVertexShader, forwardLitFragmentShader)) return false;
	if (!forwardStandardShaderProgram->registerUniform("worldMatrix")) return false;
	if (!forwardStandardShaderProgram->registerUniform("viewProjectionMatrix")) return false;

	forwardSkinnedShaderProgram = new ShaderProgram();
	if (!forwardSkinnedShaderProgram->load(skinnedTransformVertexShader, forwardLitFragmentShader)) return false;
	if (!forwardSkinnedShaderProgram->registerUniform("worldMatrix")) return false;
	if (!forwardSkinnedShaderProgram->registerUniform("viewProjectionMatrix")) return false;

	lightingPassShaderProgram = new ShaderProgram();
	if (!lightingPassShaderProgram->load(noTransformVertexShader, forwardUnlitFragmentShader)) return false;
	if (!lightingPassShaderProgram->registerUniform("center")) return false;
	if (!lightingPassShaderProgram->registerUniform("scale")) return false;

	return true;
}

bool SceneRenderer::load(const ModelManager *modelManager)
{
	assert(modelManager);
	this->modelManager = modelManager;

	if (!loadShaders())
		return false;

	if (!loadShaderPrograms())
		return false;

	return true;
}

void SceneRenderer::renderForwardPass(const Camera *camera)
{
	// clear the fbo
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the standard forward shader program
	assert(forwardStandardShaderProgram);
	glUseProgram(forwardStandardShaderProgram->getHandle());

	// set the camera projection matrix once for the shader program
	assert(camera);
	glUniformMatrix4fv(forwardStandardShaderProgram->getUniform("viewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(camera->getProjectionMatrix() * camera->getViewMatrix()));

	assert(modelManager);
	for (unsigned int i = 0; i < modelManager->getNumModels(); ++i)
	// loop through each model
	{
		// set the world matrix and render the model
		Model *model = modelManager->getModelAt(i);
		assert(model);
		glUniformMatrix4fv(forwardStandardShaderProgram->getUniform("worldMatrix"), 1, GL_FALSE, glm::value_ptr(model->getWorldMatrix()));
		model->render();
	}

	Model *testSphereModel = modelManager->getTestSphereModel();
	assert(testSphereModel);
	testSphereModel->setScale(glm::vec3(64.0f, 64.0f, 64.0f));
	glUniformMatrix4fv(forwardStandardShaderProgram->getUniform("worldMatrix"), 1, GL_FALSE, glm::value_ptr(testSphereModel->getWorldMatrix()));
	testSphereModel->render();
}

void SceneRenderer::renderGeometryPass(const Camera *camera)
{
	// TODO: implement deferred rendering
}

void SceneRenderer::renderLightingPass(const Camera *camera)
{
	// use the lighting pass shader program
	assert(lightingPassShaderProgram);
	glUseProgram(lightingPassShaderProgram->getHandle());

	// set the world matrix and render the unit quad model
	assert(modelManager);
	Model *unitQuadModel = modelManager->getUnitQuadModel();
	assert(unitQuadModel);
	Model *testSphereModel = modelManager->getTestSphereModel();
	assert(testSphereModel);
	assert(camera);

	glm::vec3 spos = testSphereModel->getPosition();

	if (camera->projectPointToScreenSpace(spos).w <= 0.0f)
	// if the light source is behind the camera
		return;

	glm::vec3 z = glm::normalize(spos - camera->getPosition());
	glm::vec3 y = glm::normalize(glm::cross(z, camera->getRight()));
	glm::vec3 x = glm::normalize(glm::cross(z, y));
	y = glm::normalize(glm::cross(z, x));
	
	float r = 64.0f;
	glm::vec3 aabbCorner[8];
	
	aabbCorner[0] = spos - x * r - y * r - z * r;
	aabbCorner[1] = spos - x * r - y * r + z * r;
	aabbCorner[2] = spos - x * r + y * r - z * r;
	aabbCorner[3] = spos - x * r + y * r + z * r;
	aabbCorner[4] = spos + x * r - y * r - z * r;
	aabbCorner[5] = spos + x * r - y * r + z * r;
	aabbCorner[6] = spos + x * r + y * r - z * r;
	aabbCorner[7] = spos + x * r + y * r + z * r;

	for (unsigned int i = 0; i < 8; ++i)
		aabbCorner[i] = glm::vec3(camera->projectPointToScreenSpace(aabbCorner[i]));

	float minX = aabbCorner[0].x;
	float maxX = aabbCorner[0].x;
	float minY = aabbCorner[0].y;
	float maxY = aabbCorner[0].y;
	for (unsigned int i = 1; i < 8; ++i)
	{
		if (aabbCorner[i].x < minX)
			minX = aabbCorner[i].x;

		if (aabbCorner[i].x > maxX)
			maxX = aabbCorner[i].x;

		if (aabbCorner[i].y < minY)
			minY = aabbCorner[i].y;

		if (aabbCorner[i].y > maxY)
			maxY = aabbCorner[i].y;
	}

	glUniform2f(lightingPassShaderProgram->getUniform("center"), minX + (maxX - minX) / 2.0f, minY + (maxY - minY) / 2.0f);
	glUniform2f(lightingPassShaderProgram->getUniform("scale"), (maxX - minX) / 2.0f, (maxY - minY) / 2.0f);
	unitQuadModel->render();
}

void SceneRenderer::render(const Camera *camera)
{
	assert(camera);
	renderForwardPass(camera);
	renderGeometryPass(camera);
	renderLightingPass(camera);
}