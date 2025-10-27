#pragma once

#include <string>

#include "Core.h"
#include "Mesh.h"
#include "Camera.h"

// dummy declaration
struct GLFWwindow;

// main application class
class Application
{
public:
	Application(const std::string& name, bool* success);
	~Application();

	// the render loop
	void Run();

public:
	// window attributes
	static constexpr uint16_t Width  = 1600;
	static constexpr uint16_t Height = 900;
	static constexpr float AspectRatio = 1.777777777f;

private:
	// basic render loop methods, called in Application::Run()
	void BeginFrame();
	void UpdateFrame();
	void DrawFrame();
	void EndFrame();

private:
	// the application window handle
	GLFWwindow* m_Window;
	Camera m_Cam;

	Ref<Shader> m_MoleculeShader;
	Ref<Shader> m_ContainerShader;

	bool m_Paused = true;
	glm::vec3 m_ClearColor;

};
