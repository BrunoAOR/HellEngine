#include <assert.h>
#include <math.h>
#include <vector>
#include "Brofiler/include/Brofiler.h"
#include "SDL/include/SDL.h"
#include "Application.h"
#include "Color.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentMaterial.h"
#include "ModuleEditorCamera.h"
#include "ModuleImGui.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "ShaderProgram.h"
#include "UpdateStatus.h"
#include "globals.h"
#include "openGL.h"

ModuleRender::ModuleRender()
{}

/* Destructor */
ModuleRender::~ModuleRender()
{}

/* Called before render is available */
bool ModuleRender::Init()
{
	LOGGER("Creating OpenGL context");
	bool ret = true;

	glContext = SDL_GL_CreateContext(App->window->window);

	if (glContext == nullptr)
	{
		LOGGER("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		if (!InitGlew())
			ret = false;
		else
		{
			if (!InitOpenGL())
				ret = false;
		}
	}

	groundGridInfo.active = true;

	matricesUBO = CreateMatricesUBO();

	return ret;
}

UpdateStatus ModuleRender::PreUpdate()
{
	Color c = App->editorCamera->camera->GetBackground();
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->editorCamera->camera->GetProjectionMatrix());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->editorCamera->camera->GetViewMatrix());

	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called every draw update */
UpdateStatus ModuleRender::Update()
{
	/* DrawGroundGrid */
	{
		if (groundGridInfo.active)
		{
			DrawGroundGrid(App->editorCamera->camera->GetPosition()[0], App->editorCamera->camera->GetPosition()[2]);
		}
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleRender::PostUpdate()
{
	BROFILER_CATEGORY("ModuleRender - GL_SwapWindow", Profiler::Color::Aqua);

	UpdateMatricesUBO();

	std::list<ComponentMaterial*> materials = GatherMaterials();
	SortMaterials(materials);
	RenderQueue(materials);
	
	App->imgui->Render();

	SDL_GL_SwapWindow(App->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

/* Called before quitting */
bool ModuleRender::CleanUp()
{
	LOGGER("Destroying OpenGL context");

	/* Destroy window */
	if (glContext != nullptr)
	{
		SDL_GL_DeleteContext(glContext);
		glContext = nullptr;
	}

	return true;
}

void ModuleRender::OnWindowResize()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
}

bool ModuleRender::IsBackFaceCullActive()
{
	if (glIsEnabled(GL_CULL_FACE))
	{
		GLint faceMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &faceMode);
		return faceMode == GL_BACK || faceMode == GL_FRONT_AND_BACK;
	}
	return false;
}

/* Initializes the GLEW library */
bool ModuleRender::InitGlew() const
{
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LOGGER("Error: glew could not be initialized. glewInit error: %s", glewGetErrorString(err));
		return false;
	}

	LOGGER("GLEW: Using Glew %s", glewGetString(GLEW_VERSION));

	LOGGER("Vendor: %s", glGetString(GL_VENDOR));
	LOGGER("Renderer: %s", glGetString(GL_RENDERER));
	LOGGER("OpenGL version supported %s", glGetString(GL_VERSION));
	LOGGER("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	return true;
}

/* Initializes OpenGL */
bool ModuleRender::InitOpenGL() const
{
	/* Reset projection and modelview matrices */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Setup OpenGL options */
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);

	glFrontFace(GL_CCW);
	//glCullFace(GL_BACK);

	/* Set viewport */
	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	/* Check for errors */
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		LOGGER("Error initializing OpenGL! %s\n", glewGetErrorString(error));
		return false;
	}
	return true;
}

void ModuleRender::DrawGroundGrid(float xOffset, float zOffset, int halfSize) const
{
	if (!groundGridInfo.tracking)
	{
		xOffset = 0;
		zOffset = 0;
	}
	else if (!groundGridInfo.continuousTracking)
	{
		xOffset = round(xOffset);
		zOffset = round(zOffset);
	}

	int start = -halfSize;
	int extent = halfSize;
	float y = 0;

	glBegin(GL_LINES);

	for (int i = start; i <= extent; i++)
	{
		if (i == start) {
			glColor3f(.6f, .3f, .3f);
		}
		else {
			glColor3f(.25f, .25f, .25f);
		}

		glVertex3f((float)i + xOffset, y, (float)start + zOffset);
		glVertex3f((float)i + xOffset, y, (float)extent + zOffset);

		if (i == start) {
			glColor3f(.3f, .3f, .6f);
		}
		else {
			glColor3f(.25f, .25f, .25f);
		}

		glVertex3f((float)start + xOffset, y, (float)i + zOffset);
		glVertex3f((float)extent + xOffset, y, (float)i + zOffset);
	}

	glEnd();
}

uint ModuleRender::CreateMatricesUBO()
{
	uint ubo = 0;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	int bufferSize = 2 * 16 * sizeof(float);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
	glBindBufferRange(GL_UNIFORM_BUFFER, matricesUBOBindingPoint, ubo, 0, bufferSize);
	return ubo;
}

void ModuleRender::UpdateMatricesUBO()
{
	glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, 16 * sizeof(float), App->editorCamera->camera->GetProjectionMatrix());
	glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 16 * sizeof(float), App->editorCamera->camera->GetViewMatrix());
	glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE);
}

std::list<ComponentMaterial*> ModuleRender::GatherMaterials()
{
	std::list<ComponentMaterial*> materials;

	std::stack<GameObject*> stack;
	stack.push(App->scene->root);
	GameObject* go = nullptr;

	while (!stack.empty())
	{
		go = stack.top();
		stack.pop();

		std::vector<Component*> materialsAsComponent = go->GetComponents(ComponentType::MATERIAL);
		for (Component* component : materialsAsComponent)
		{
			ComponentMaterial* material = (ComponentMaterial*)component;
			if (material->GetActive() && PassesVailidityTest(material) && PassesFrustumCulling(material))
			{
				materials.push_back(material);
			}
		}

		std::vector<GameObject*> children = go->GetChildren();
		for (GameObject* child : children)
			stack.push(child);
;	}

	return materials;
}

bool ModuleRender::PassesVailidityTest(const ComponentMaterial* material)
{
	ComponentMesh* mesh = (ComponentMesh*)material->gameObject->GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)material->gameObject->GetComponent(ComponentType::TRANSFORM);
	return material->IsValid() && mesh && transform;
}

bool ModuleRender::PassesFrustumCulling(const ComponentMaterial* material)
{
	ComponentMesh* mesh = (ComponentMesh*)material->gameObject->GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)material->gameObject->GetComponent(ComponentType::TRANSFORM);

	bool insideFrustum = true;

	ComponentCamera* editorCamera = App->editorCamera->camera;
	if (editorCamera != nullptr)
	{
		if (App->scene->UsingQuadTree() && transform->GetIsStatic())
			insideFrustum = editorCamera->IsInsideFrustum(material->gameObject);
		else
			insideFrustum = transform->GetBoundingBox().Contains(editorCamera->GetFrustum());

	}
	if (insideFrustum) {

		ComponentCamera* activeGameCamera = App->scene->GetActiveGameCamera();

		if (activeGameCamera != nullptr && activeGameCamera->FrustumCulling())
		{
			if (App->scene->UsingQuadTree() && transform->GetIsStatic())
				insideFrustum = activeGameCamera->IsInsideFrustum(material->gameObject);
			else
				insideFrustum = transform->GetBoundingBox().Contains(activeGameCamera->GetFrustum());

		}
	}

	return insideFrustum;
}

void ModuleRender::SortMaterials(std::list<ComponentMaterial*>& materials)
{
	/* Sort by shaderProgram */
	materials.sort([](ComponentMaterial* mat1, ComponentMaterial* mat2) {
		return mat1->shaderProgram < mat2->shaderProgram;
	});
}

void ModuleRender::RenderQueue(std::list<ComponentMaterial*>& materials)
{
	const ShaderProgram* shaderProgram = nullptr;
	for (ComponentMaterial* material : materials)
	{
		if (!shaderProgram || shaderProgram != material->shaderProgram)
		{
			if (shaderProgram)
				shaderProgram->Deactivate();
			shaderProgram = material->shaderProgram;
			shaderProgram->Activate();
		}

		DrawMaterial(material);
	}
	if (shaderProgram)
		shaderProgram->Deactivate();
}

void ModuleRender::DrawMaterial(ComponentMaterial* material)
{
	ComponentMesh* mesh = (ComponentMesh*)material->gameObject->GetComponent(ComponentType::MESH);
	ComponentTransform* transform = (ComponentTransform*)material->gameObject->GetComponent(ComponentType::TRANSFORM);
	const ModelInfo* modelInfo = mesh->GetActiveModelInfo();

	if (modelInfo != nullptr && modelInfo->meshInfosIndexes.size() > 0)
	{
		const float* modelMatrix = transform->GetModelMatrix();

		material->shaderProgram->UpdateModelMatrixUniform(modelMatrix);

		float3 unused;
		Quat rotQuat;
		DecomposeMatrix(transform->GetModelMatrix4x4(), unused, rotQuat, unused);
		float4x4 normalMatrix = float4x4::QuatToRotation(rotQuat).Transposed();

		const float* lightPos = nullptr;
		if (ComponentLight* light = App->scene->GetActiveGameLight())
			lightPos = light->GetPosition()->ptr();

		material->shaderProgram->UpdateLightingUniforms(normalMatrix.ptr(), lightPos, App->editorCamera->camera->GetPosition());

		material->UpdatePublicUniforms();

		if (material->modelInfoVaoIndex >= 0 && material->modelInfoVaoIndex < (int)modelInfo->meshInfosIndexes.size())
		{
			unsigned int meshInfoIndex = modelInfo->meshInfosIndexes.at(material->modelInfoVaoIndex);
			const MeshInfo* meshInfo = App->scene->meshes.at(meshInfoIndex);
			const std::map<const MeshInfo*, float4x4[MAX_BONES]> bonesPalettes = mesh->GetBonesPalletes();
			const float4x4* bonesPalette = bonesPalettes.at(meshInfo);
			material->shaderProgram->UpdateBonesUniform(bonesPalette[0].ptr());
			DrawMeshInfo(material, meshInfo);
		}
		else if (material->modelInfoVaoIndex == -1)
		{
			/* This case occurs when the Material doesn't correspond to a Mesh loaded from a model */
			for (unsigned int meshInfoIndex : modelInfo->meshInfosIndexes)
			{
				const MeshInfo* meshInfo = App->scene->meshes.at(meshInfoIndex);
				DrawMeshInfo(material, meshInfo);
			}
		}
	}
}

void ModuleRender::DrawMeshInfo(const ComponentMaterial* material, const MeshInfo* meshInfo)
{
	glBindTexture(GL_TEXTURE_2D, material->diffuseBufferId);
	if (material->normalBufferId != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->normalBufferId);
	}
	if (material->specularBufferId != 0)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material->specularBufferId);
	}

	GLint texLoc;
	texLoc = glGetUniformLocation(material->shaderProgram->GetProgramId(), "ourTexture");
	glUniform1i(texLoc, 0);
	texLoc = glGetUniformLocation(material->shaderProgram->GetProgramId(), "ourNormal");
	glUniform1i(texLoc, 1);
	texLoc = glGetUniformLocation(material->shaderProgram->GetProgramId(), "ourSpecular");
	glUniform1i(texLoc, 2);

	glBindVertexArray(meshInfo->vao);
	glDrawElements(GL_TRIANGLES, meshInfo->elementsCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(GL_NONE);

	if (material->specularBufferId != 0)
	{
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
	}
	if (material->normalBufferId != 0)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GL_NONE);
		glActiveTexture(GL_TEXTURE0);
	}

	glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void ModuleRender::ToggleOpenGLCapability(bool state, GLenum cap)
{
	if (state)
	{
		glEnable(cap);
	}
	else
	{
		glDisable(cap);
	}
}

void ModuleRender::SetFogMode(GLenum fogMode)
{
	glFogi(GL_FOG_MODE, fogMode);
}

void ModuleRender::SetFogDensity(float density)
{
	glFogf(GL_FOG_DENSITY, density);
}

void ModuleRender::setFogStartAndEnd(float start, float end)
{
	glFogf(GL_FOG_START, start);
	glFogf(GL_FOG_END, end);
}

void ModuleRender::SetFogColor(float * color)
{
	glFogfv(GL_FOG_COLOR, color);
}

void ModuleRender::SetAmbientLightColor(float * color)
{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color);
}

void ModuleRender::SetLightColor(GLenum lightSymbolicName, float * color)
{
	glLightfv(lightSymbolicName, GL_AMBIENT, color);
}

void ModuleRender::SetPolygonDrawMode(GLenum drawMode)
{
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
}
