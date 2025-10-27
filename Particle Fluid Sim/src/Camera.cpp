#include "Camera.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

static constexpr float pi = 3.1415926f;
static constexpr float tau = 6.2831853f;

Camera::Camera(float aspectRatio)
	:
	theta(pi / 2.0f),
	phi(pi / 2.0f)
{
	m_Position = glm::vec3(0.0f, 0.0f, 50.0f);
	m_View = glm::lookAt(m_Position, glm::vec3(0.0f), Camera::Up);
	m_Projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

void Camera::Update(float dt, GLFWwindow* pwnd)
{
	if (glfwGetKey(pwnd, GLFW_KEY_E) == GLFW_PRESS) {
		theta -= 0.8f * dt;
	}
	if (glfwGetKey(pwnd, GLFW_KEY_Q) == GLFW_PRESS) {
		theta += 0.8f * dt;
	}
	if (glfwGetKey(pwnd, GLFW_KEY_W) == GLFW_PRESS) {
		phi -= 0.8f * dt;
	}
	if (glfwGetKey(pwnd, GLFW_KEY_S) == GLFW_PRESS) {
		phi += 0.8f * dt;
	}
	theta = std::fmod(theta, tau);
	phi = std::fmod(phi, pi);

	const float radius = 50.0f;
	m_Position.x = radius * std::cosf(theta) * std::sinf(phi);
	m_Position.z = radius * std::sinf(theta) * std::sinf(phi);
	m_Position.y = radius * std::cosf(phi);
	m_View = glm::lookAt(m_Position, glm::vec3(0.0f), Camera::Up);
}

glm::mat4 Camera::GetProjection()
{
	return m_Projection;
}

glm::mat4 Camera::GetView()
{
	return m_View;
}
