#pragma once

#include <glew/glew.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

// basic shader type
class Shader
{
public:
	Shader() = default;
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	~Shader();

	void Use();

	void SetUniformFloat(const std::string& name, float value);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4& mat);

private:
	GLuint m_ID;

};