#include "Renderer.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include "Random.h"
#include "SPHSolver.h"

#include <iostream>

static struct ImGuiData
{
	ImGuiIO io;
	bool Demo;
	bool Telemetry;
	bool Controls;
} UIdata;

static struct SceneData
{
	Ref<Cube> Container;
	Ref<Sphere> MoleculeMesh;  // only one instance of the mesh needed

	glm::vec3 ContainerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	float ContainerRotation = 0.0f;
	glm::vec3 ContainerScale = glm::vec3(41.0f, 23.0f, 1.0f);
	glm::vec3 BoxPosition = glm::vec3(-16.0f, 0.0f, 0.0f);
	glm::vec3 BoxScale = glm::vec3(7.0f, 21.0f, 1.0f);
	float MoleculeScale = 0.515f;
	float InfluenceRadius = 0.5f;
	float Viscosity = 1.0f;
	float Delta = 0.001666f;
} Sdata;

void Renderer::UI::Init(GLFWwindow** window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	UIdata.io = ImGui::GetIO();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(*window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	UIdata.Demo = false;
	UIdata.Telemetry = false;
	UIdata.Controls = false;
}

void Renderer::UI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::UI::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	UIdata.io = ImGui::GetIO();
}

// TODO: Add Start, Stop, Reset buttons
void Renderer::UI::UpdateFrame(bool* paused)
{
	ImGui::Begin("Scene Buttons");
	ImVec2 cursor = ImGui::GetCursorPos();
	if (ImGui::SmallButton("Resume")) {
		*paused = false;
	}
	ImGui::SetCursorPos({ cursor.x + 50.0f, cursor.y });
	if (ImGui::SmallButton("Pause")) {
		*paused = true;
	}
	ImGui::SetCursorPos({ cursor.x + 93.0f, cursor.y });
	if (ImGui::SmallButton("Reset")) {
		*paused = true;
		SPHSolver::ResetMolecules();
	}
	ImGui::End();

	if (UIdata.Controls) {
		ImGui::Begin("Scene controls", &UIdata.Controls);
		ImGui::SliderFloat2("Container Position", &Sdata.ContainerPosition[0], -10.0f, 10.0f);
		ImGui::SliderFloat ("Container Rotation", &Sdata.ContainerRotation, 0.0f, 360.0f);
		ImGui::SliderFloat3("Container Scale", &Sdata.ContainerScale[0], 0.0f, 60.0f);
		ImGui::SliderFloat2("Box Position", &Sdata.BoxPosition[0], -20.0f, 20.0f);
		ImGui::SliderFloat2("Box Scale", &Sdata.BoxScale[0], 0.0f, 40.0f);
		ImGui::SliderFloat("Molecule scale", &Sdata.MoleculeScale, 0.001f, 1.0f);
		ImGui::SliderFloat("Influence Radius", &Sdata.InfluenceRadius, 0.1f, 2.0f);
		ImGui::SliderFloat("Viscosity", &Sdata.Viscosity, 0.0f, 10.0f);
		ImGui::SliderFloat("Delta Time", &Sdata.Delta, 0.0001f, 0.002f);
		ImGui::End();

		Sdata.MoleculeMesh->SetScale(glm::vec3(Sdata.MoleculeScale));
	}
}

void Renderer::UI::Draw()
{
	if (UIdata.Demo) {
		Renderer::UI::DemoWindow(&UIdata.Demo);
	}
	if (UIdata.Telemetry) {
		Renderer::UI::TelemetryWindow(&UIdata.Telemetry);
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::UI::ShowWindowType(Renderer::UI::WindowTypes type)
{
	switch (type)
	{
	case Renderer::UI::WindowTypes::DEMO: UIdata.Demo = true; return;
	case Renderer::UI::WindowTypes::TELEMETRY: UIdata.Telemetry = true; return;
	case Renderer::UI::WindowTypes::CONTROLS: UIdata.Controls = true; return;
	default: std::cout << "Error Renderer::UI::ShowDemoWindow: Invalid window type" << std::endl;
	}
}

void Renderer::UI::TelemetryWindow(bool* popen)
{
	ImGui::Begin("Scene Telemetry", popen);
	ImGui::Text("Application FPS: %.2f (%.2f ms / frame)", UIdata.io.Framerate, 1000.0f / UIdata.io.Framerate);
	ImGui::Text("Number of Quads: %lu", Renderer::Scene::NumMolecules + 1);
	ImGui::Text("Container Quads: 1");
	ImGui::Text("Number of molecules: %lu (%lu draw calls)", Renderer::Scene::NumMolecules, Renderer::Scene::NumMolecules);
	ImGui::End();
}

float Renderer::UI::GetDeltaTime()
{
	return UIdata.io.DeltaTime;
}

void Renderer::Scene::Init()
{
	Sdata.Container = std::make_shared<Cube>();
	Sdata.MoleculeMesh = std::make_shared<Sphere>(16, 16);
	Sdata.Container->SetScale(glm::vec3(Sdata.ContainerScale.x, Sdata.ContainerScale.y, 1.0f));
	Sdata.Container->SetTranslation(glm::vec3(0.0f));

	Sdata.MoleculeMesh->SetScale(glm::vec3(Sdata.MoleculeScale));
	Sdata.MoleculeMesh->SetRotation(0.0f);
}

void Renderer::Scene::Render(Ref<Shader>& containerShader, Ref<Shader>& moleculeShader, bool paused)
{
	containerShader->Use();
	Sdata.Container->SetTranslation(Sdata.ContainerPosition);
	Sdata.Container->SetRotation(Sdata.ContainerRotation);
	Sdata.Container->SetScale(Sdata.ContainerScale);
	containerShader->SetUniformMatrix4f("u_Model", Sdata.Container->GetTransform());
	glBindVertexArray(Sdata.Container->GetVAO());
	glDrawElements(GL_LINE_LOOP, (GLsizei)Sdata.Container->GetIndices().size(), GL_UNSIGNED_INT, nullptr);
	
	// draw the outline of the starting box of fluid
	if (paused) {
		Sdata.Container->SetTranslation(Sdata.BoxPosition);
		Sdata.Container->SetRotation(0.0f);
		Sdata.Container->SetScale(Sdata.BoxScale);
		containerShader->SetUniformMatrix4f("u_Model", Sdata.Container->GetTransform());
		glBindVertexArray(Sdata.Container->GetVAO());
		glDrawElements(GL_LINE_LOOP, (GLsizei)Sdata.Container->GetIndices().size(), GL_UNSIGNED_INT, nullptr);
	}
	
	// then go through all the molecules and render them normally
	std::vector<SPHSolver::MoleculeProperties> properties = SPHSolver::GetProperties();
	moleculeShader->Use();
	glBindVertexArray(Sdata.MoleculeMesh->GetVAO());
	for (uint32_t i = 0; i < Renderer::Scene::NumMolecules; i++) {
		const SPHSolver::MoleculeProperties& p = properties[i];
		Sdata.MoleculeMesh->SetTranslation(p.Position);
		moleculeShader->SetUniformMatrix4f("u_Model", Sdata.MoleculeMesh->GetTransform());
		moleculeShader->SetUniformFloat("u_SpeedSq", glm::dot(p.Velocity, p.Velocity));
		glDrawElements(GL_TRIANGLES, (GLsizei)Sdata.MoleculeMesh->GetIndices().size(), GL_UNSIGNED_INT, nullptr);
	}
}

const glm::vec3 Renderer::Scene::GetContainerBounds()
{
	glm::vec3 result;
	result.x =  0.5f * Sdata.ContainerScale.x;
	result.y = -0.5f * Sdata.ContainerScale.y;
	result.z = 1.0f;
	return result;
}
float Renderer::Scene::GetInfluenceRadius()
{
	return Sdata.InfluenceRadius;
}

float Renderer::Scene::GetMoleculeScale()
{
	return Sdata.MoleculeScale;
}

float Renderer::Scene::GetViscosityStrength()
{
	return Sdata.Viscosity;
}

glm::mat4 Renderer::Scene::GetStartingBoxData()
{
	glm::mat4 result;
	result[0] = glm::vec4(Sdata.BoxPosition.x, Sdata.BoxPosition.y, 0.0f, 0.0f);
	result[1] = glm::vec4(Sdata.BoxScale.x, Sdata.BoxScale.y, 1.0f, 0.0f);
	return result;
}

glm::mat4 Renderer::Scene::GetContainerTransform()
{
	return Sdata.Container->GetTransform();
}

float Renderer::Scene::GetContainerRotation()
{
	return Sdata.ContainerRotation;
}

float Renderer::Scene::GetDeltaTime()
{
	return Sdata.Delta;
}
