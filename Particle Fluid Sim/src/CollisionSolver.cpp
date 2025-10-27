#include "CollisionSolver.h"

#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void CollisionSolver::ContainerCollision(SPHSolver::MoleculeProperties& props, float moleculeScale)
{
	//ContainerTransform = Renderer::Scene::GetContainerTransform();
	glm::mat4 ContainerTransform = Renderer::Scene::GetContainerTransform();
	if (std::fabsf(glm::determinant(ContainerTransform)) < 0.0001f) {
		return;
	}
	ContainerTransform = glm::inverse(ContainerTransform);
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-Renderer::Scene::GetContainerRotation()), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec4 position(props.Position.x, props.Position.y, props.Position.z, 1.0f);
	glm::vec4 velocity(props.Velocity.x, props.Velocity.y, props.Velocity.z, 0.0f);

	velocity = rotation * velocity;
	position = ContainerTransform * position;

	float dampness = 0.5f;
	if (position.y < -0.5f && position.x > -0.6f && position.x < 0.6f
		&& position.z > -0.6f && position.z < 0.6f) {
		velocity.y = -dampness * velocity.y;
		position.y = -0.5f;
	}
	if (position.y > 0.5f && position.x > -0.6f && position.x < 0.6f
		&& position.z > -0.6f && position.z < 0.6f) {
		velocity.y = -dampness * velocity.y;
		position.y = 0.5f;
	}
	if (position.x < -0.5f && position.y > -0.6f && position.y < 0.6f
		&& position.z > -0.6f && position.z < 0.6f) {
		velocity.x = -dampness * velocity.x;
		position.x = -0.5f;
	}
	if (position.x > 0.5f && position.y > -0.6f && position.y < 0.6f
		&& position.z > -0.6f && position.z < 0.6f) {
		velocity.x = -dampness * velocity.x;
		position.x = 0.5f;
	}
	if (position.z < -0.5f && position.y > -0.6f && position.y < 0.6f
		&& position.x > -0.6f && position.x < 0.6f) {
		velocity.z = -dampness * velocity.z;
		position.z = -0.5f;
	}
	if (position.z > 0.5f && position.y > -0.6f && position.y < 0.6f
		&& position.x > -0.6f && position.x < 0.6f) {
		velocity.z = -dampness * velocity.z;
		position.z = 0.5f;
	}

	ContainerTransform = Renderer::Scene::GetContainerTransform();
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(Renderer::Scene::GetContainerRotation()), glm::vec3(0.0f, 0.0f, 1.0f));
	props.Velocity = rotation * velocity;
	props.Position = ContainerTransform * position;
}
