#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core.h"
#include "Shader.h"

// mesh class that holds necessary data
class Mesh
{
public:
	Mesh(const glm::vec3& translation, const glm::vec3& scale);
	virtual ~Mesh();

	GLuint GetVAO();
	const std::vector<GLuint>& GetIndices();
	const glm::mat4 GetTransform();
	void SetTranslation(const glm::vec3& translation);
	void SetRotation(float angle);
	void SetScale(const glm::vec3& scale);

	// takes the position of the center of the mesh
	// so for each position there are multiple vertices
	virtual void AddVertices(const std::vector<glm::vec3>& positions) {}

public:
	static constexpr size_t MaxQuadsPerBatch = 100'000;

protected:
	// the characteristics of the mesh as a body
	struct PhysicsProperties
	{
		glm::vec3 Translation;  // or position
		glm::vec3 Scale;
		float Angle;
		// no need for rotation yet
	};

	// vertex characteristics
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
	};

	// initializes the buffers with their data
	void SetupMesh();

protected:
	std::vector<Vertex> m_Vertices;
	std::vector<GLuint> m_Indices;
	PhysicsProperties m_Props;


protected:
	GLuint m_VAO, m_VBO, m_EBO;

};

// a special mesh type
class Quad : public Mesh
{
public:
	Quad(const glm::vec3& translation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

};

class Cube : public Mesh
{
public:
	Cube(const glm::vec3& translation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
};

class Sphere : public Mesh
{
public:
	Sphere(int vdivs, int hdivs, const glm::vec3& translation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));
};
