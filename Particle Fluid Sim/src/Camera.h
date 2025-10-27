#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct GLFWwindow;

// basic camera class, does not suppot movement
class Camera
{
public:
	Camera(float aspectRatio);

	void Update(float dt, GLFWwindow* pwnd);

	glm::mat4 GetProjection();
	glm::mat4 GetView();

public:
	static constexpr glm::vec3 Up = { 0.0f, 1.0f, 0.0f };

private:
	glm::mat4 m_View;
	glm::mat4 m_Projection;

	glm::vec3 m_Position;

	float theta;
	float phi;

};