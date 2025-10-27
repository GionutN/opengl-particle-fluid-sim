#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

// retrieves the shader code from files and binds them into a single program
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
	std::string vertexCode, fragmentCode;
	std::ifstream vertStream, fragStream;

	// enable reading and opening exceptions
	vertStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertStream.open(vertexPath);
		fragStream.open(fragmentPath);
		std::stringstream vss, fss;

		// retrieve the vertex shader code
		vss << vertStream.rdbuf();
		vertexCode = vss.str();

		// retrieve the fragment shader code
		fss << fragStream.rdbuf();
		fragmentCode = fss.str();

	}
	catch (std::ifstream::failure e) {
		// print the caught error
		std::cout << "Error Shader::Shader " << e.code().value() << std::endl;
		std::cout << e.what() << std::endl;
	}
	const char* vSource = vertexCode.c_str();
	const char* fSource = fragmentCode.c_str();
	
	// create and compile the shaders
	int success;
	const int infoLen = 512;
	char info[infoLen];

	// vertex shader
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vSource, nullptr);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		// print compile errors
		glGetShaderInfoLog(vertex, infoLen, nullptr, info);
		std::cout << "Error Shader::Shader: Vertex Shader compilation error " << std::endl;
		std::cout << info << std::endl;
	}

	// fragment shader
	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fSource, nullptr);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		// print compile errors
		glGetShaderInfoLog(fragment, infoLen, nullptr, info);
		std::cout << "Error Shader::Shader: Fragment Shader compilation error " << std::endl;
		std::cout << info << std::endl;
	}

	// bind the shaders in a single program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success) {
		// print linking errors
		glGetProgramInfoLog(m_ID, infoLen, nullptr, info);
		std::cout << "Error Shader::Shader: Shader program linking error " << std::endl;
		std::cout << info << std::endl;
	}

	// the individuals shaders are not needed anymore
	// only the shader program is used
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

void Shader::Use()
{
	// bind the shader
	glUseProgram(m_ID);
}

void Shader::SetUniformFloat(const std::string& name, float value)
{
	this->Use();
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::SetUniformMatrix4f(const std::string& name, const glm::mat4& mat)
{
	this->Use();
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
