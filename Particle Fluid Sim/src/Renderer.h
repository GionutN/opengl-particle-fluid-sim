#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Mesh.h"

struct GLFWwindow;

class Renderer
{
public:


	class UI
	{
	public:
		enum class WindowTypes
		{
			INVALID = -1,
			DEMO,
			TELEMETRY,
			CONTROLS,
			NUMWINDOWTYPES
		};

	public:
		static void Init(GLFWwindow** window);
		static void Shutdown();

		static void BeginFrame();
		static void UpdateFrame(bool* paused);
		static void Draw();

		static void ShowWindowType(Renderer::UI::WindowTypes type);

		inline static void DemoWindow(bool* popen) { ImGui::ShowDemoWindow(popen); }
		static void TelemetryWindow(bool* popen);
		
		static float GetDeltaTime();

	private:
		UI() = default;

	};

	class Scene
	{
	public:
		static void Init();
		
		static void Render(Ref<Shader>& containershd, Ref<Shader>& moleculeshd, bool paused);

		static const glm::vec3 GetContainerBounds();
		static float GetInfluenceRadius();
		static float GetMoleculeScale();
		static float GetViscosityStrength();
		static glm::mat4 GetStartingBoxData();
		static glm::mat4 GetContainerTransform();
		static float GetContainerRotation();
		static float GetDeltaTime();

	public:
		static constexpr uint32_t NumMolecules = 2048;

	private:
		Scene() = default;

	};

private:
	Renderer() = default;

};