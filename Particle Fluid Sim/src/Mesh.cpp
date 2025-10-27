#include "Mesh.h"

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <iostream>

static constexpr float pi = 3.1415926f;
static constexpr float tau = 6.2831853f;

Mesh::Mesh(const glm::vec3& translation, const glm::vec3& scale)
{
	m_Props.Translation = translation;
	m_Props.Scale = scale;
}

Mesh::~Mesh()
{
	// frees the allocated resources
	glDeleteBuffers(1, &m_EBO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}

GLuint Mesh::GetVAO()
{
	return m_VAO;
}

const std::vector<GLuint>& Mesh::GetIndices()
{
	return m_Indices;
}

void Mesh::SetTranslation(const glm::vec3& translation)
{
	m_Props.Translation = translation;
}

void Mesh::SetRotation(float angle)
{
	m_Props.Angle = angle;
}

void Mesh::SetScale(const glm::vec3& scale)
{
	m_Props.Scale = scale;
}

const glm::mat4 Mesh::GetTransform()
{
	static float time = 0.0f;
	time += 0.00001f;
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, m_Props.Translation);
	transform = glm::rotate(transform, glm::radians(m_Props.Angle), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, m_Props.Scale);
	return transform;
}

void Mesh::SetupMesh()
{
	// generate the vertex array that holds the vertex buffer and element buffer
	glGenVertexArrays(1, &m_VAO);

	// generate the vertex and element buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	// vertex buffer data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

	//element buffer data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(GLuint), m_Indices.data(), GL_STATIC_DRAW);

	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
	glEnableVertexAttribArray(0);
	//normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

Quad::Quad(const glm::vec3& translation, const glm::vec3& scale)
	:
	Mesh(translation, scale)
{
	glm::vec3 normal = { 0.0f, 0.0f, 1.0f };
	m_Vertices = {
		{ glm::vec3(-0.5f, -0.5f, 0.0f), normal },
		{ glm::vec3( 0.5f, -0.5f, 0.0f), normal },
		{ glm::vec3( 0.5f,  0.5f, 0.0f), normal },
		{ glm::vec3(-0.5f,  0.5f, 0.0f), normal }
	};
	m_Indices = {
		0, 1, 2,
		2, 3, 0
	};
	SetupMesh();
}

Cube::Cube(const glm::vec3& translation, const glm::vec3& scale)
	:
	Mesh(translation, scale)
{
	m_Vertices = {
		//            x      y      z                 x      y      z
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f)},

		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f)},

		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f)},
		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f)},

		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f)},

		{glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f)},
		{glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f)},
		{glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f)},
		{glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f)},

		{glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f)},
		{glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f)},
		{glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f)},
		{glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f)},
	};

	m_Indices = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};

	SetupMesh();
}

Sphere::Sphere(int vdivs, int hdivs, const glm::vec3& translation, const glm::vec3& scale)
	:
	Mesh(translation, scale)
{
	for (float phi = 0.0f; phi < pi + 0.001f; phi += pi / vdivs) {
		for (float theta = 0.0f; theta < tau + 0.001f; theta += tau / hdivs) {
			glm::vec3 pos;
			pos.x = std::sinf(phi) * std::cosf(theta);
			pos.y = std::sinf(phi) * std::sinf(theta);
			pos.z = std::cosf(phi);
			m_Vertices.push_back({ 0.5f * pos, pos});
		}
	}

	for (int i = 0; i < m_Vertices.size() - hdivs; i++) {
		m_Indices.push_back(i);
		m_Indices.push_back((i + 1) / hdivs * hdivs + (i + 1) % hdivs);
		m_Indices.push_back((i + 1) / hdivs * hdivs + (i + 1) % hdivs + hdivs);

		m_Indices.push_back((i + 1) / hdivs * hdivs + (i + 1) % hdivs + hdivs);
		m_Indices.push_back(i + hdivs);
		m_Indices.push_back(i);

	}

	SetupMesh();
}
