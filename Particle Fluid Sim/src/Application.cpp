#include "Application.h"

#include <iostream>

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include "Renderer.h"
#include "Random.h"
#include "SPHSolver.h"

// returns an error code as a c-style out parameter
Application::Application(const std::string& name, bool* success)
	:
	m_Cam(Application::AspectRatio)
{
	*success = true;
	int result;

	// initialize and configure glfw
	result = glfwInit();
	if (result != GLFW_TRUE) {
		std::cout << "Error Application::Application: Failed to initialize GLFW" << std::endl;
		*success = false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// create and configure the application window
	m_Window = glfwCreateWindow(Application::Width, Application::Height, name.c_str(), nullptr, nullptr);
	if (m_Window == nullptr)
	{
		std::cout << "Error Application::Application: Could not create a GLFW window" << std::endl;
		*success = false;
	}
	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(0);	// deactivate v-sync

	// get all the OpenGL function pointers
	result = glewInit();
	if (result != GLEW_OK)
	{
		std::cout << "Error Application::Application: Failed to initialize GLEW" << std::endl;
		*success = false;
	}

	Random::Init();
	Renderer::UI::Init(&m_Window);
	Renderer::Scene::Init();
	SPHSolver::Init();

	m_ClearColor[0] = m_ClearColor[1] = m_ClearColor[2] = 0.1f;

	std::string vShaderPath = "..\\Particle Fluid Sim\\Assets\\Shaders\\VMolSphereShader.glsl";
	std::string fShaderPath = "..\\Particle Fluid Sim\\Assets\\Shaders\\FMolSphereShader.glsl";
	m_MoleculeShader = std::make_shared<Shader>(vShaderPath, fShaderPath);

	vShaderPath = "..\\Particle Fluid Sim\\Assets\\Shaders\\VContainerShader.glsl";
	fShaderPath = "..\\Particle Fluid Sim\\Assets\\Shaders\\FContainerShader.glsl";
	m_ContainerShader = std::make_shared<Shader>(vShaderPath, fShaderPath);

	m_MoleculeShader->SetUniformMatrix4f("u_Projection", m_Cam.GetProjection());
	m_MoleculeShader->SetUniformMatrix4f("u_View", m_Cam.GetView());
	m_ContainerShader->SetUniformMatrix4f("u_Projection", m_Cam.GetProjection());
	m_ContainerShader->SetUniformMatrix4f("u_View", m_Cam.GetView());

}

Application::~Application()
{
	Renderer::UI::Shutdown();

	// free all the resources allocated by glfw
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Application::Run()
{
	// the render loop
	while (!glfwWindowShouldClose(m_Window)) {
		BeginFrame();
		UpdateFrame();
		DrawFrame();
		EndFrame();
	}
}

void Application::BeginFrame()
{
	Renderer::UI::BeginFrame();

	// clear the frame
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

// updates the logic
void Application::UpdateFrame()
{
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
		Renderer::UI::ShowWindowType(Renderer::UI::WindowTypes::DEMO);
	}
	if (glfwGetKey(m_Window, GLFW_KEY_T) == GLFW_PRESS) {
		Renderer::UI::ShowWindowType(Renderer::UI::WindowTypes::TELEMETRY);
	}
	if (glfwGetKey(m_Window, GLFW_KEY_C) == GLFW_PRESS) {
		Renderer::UI::ShowWindowType(Renderer::UI::WindowTypes::CONTROLS);
	}

	m_Cam.Update(Renderer::UI::GetDeltaTime(), m_Window);

	Renderer::UI::UpdateFrame(&m_Paused);

	// update the properties of all molecules
	if (!m_Paused) {
		uint32_t numIters = 7;
		for (uint32_t i = 0; i < numIters; i++) {
			SPHSolver::Update(Renderer::UI::GetDeltaTime() / numIters, Renderer::Scene::GetContainerBounds());
		}
	}

	m_MoleculeShader->SetUniformMatrix4f("u_View", m_Cam.GetView());
	m_ContainerShader->SetUniformMatrix4f("u_View", m_Cam.GetView());

}

// draws the frame based on the new logic
void Application::DrawFrame()
{
	Renderer::Scene::Render(m_ContainerShader, m_MoleculeShader, m_Paused);
	Renderer::UI::Draw();
}

// ends the frame
void Application::EndFrame()
{
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}
