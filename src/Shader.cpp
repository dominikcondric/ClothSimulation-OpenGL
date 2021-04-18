#include <glad/glad.h>
#include <sstream>
#include <fstream>
#include <string>
#include <iostream>
#include "Shader.h"
#include "PathConfig.h"

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;

	vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexShaderFile.open(SHADER_FOLDER + vertexShader);
		fragmentShaderFile.open(SHADER_FOLDER + fragmentShader);
		std::stringstream vertexShaderStream, fragmentShaderStream;

		vertexShaderStream << vertexShaderFile.rdbuf();
		fragmentShaderStream << fragmentShaderFile.rdbuf();

		vertexCode = vertexShaderStream.str();
		fragmentCode = fragmentShaderStream.str();
	}
	catch (std::ifstream::failure& e) {
		std::cout << "Error: File not successfully read/found!" << std::endl;
	}

	GLint success;
	char infoLog[512];
	const char* vertexCodeString = vertexCode.c_str();
	const char* fragmentCodeString = fragmentCode.c_str();
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderID, 1, &vertexCodeString, NULL);
	glCompileShader(vertexShaderID);
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShaderID, 512, nullptr, infoLog);
		std::cout << "Failed to compile vertex shader : " << vertexShader << " " << infoLog << std::endl;
	}

	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentCodeString, NULL);
	glCompileShader(fragmentShaderID);
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShaderID, 512, nullptr, infoLog);
		std::cout << "Failed to compile fragment shader : " << fragmentShader << " " << infoLog << std::endl;
	}

	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShaderID);
	glAttachShader(shaderID, fragmentShaderID);
	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
		std::cout << "Failed to link program : " << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

Shader::~Shader()
{
	glDeleteProgram(shaderID);
}

GLint Shader::getUniformLocation(const std::string& name) const
{
	if ((uniformCache.find(name)) != uniformCache.end()) return uniformCache[name];

	GLint location = glGetUniformLocation(shaderID, name.c_str());
	if (location != -1) {
		uniformCache.insert(std::make_pair(name, location));
	}

	return location;
}

void Shader::use() const
{
	glUseProgram(shaderID);
}

void Shader::setBool(const std::string& name, const bool value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setBoolArray(const std::string& name, GLsizei count, bool values[]) const
{
	glUniform1iv(getUniformLocation(name), count, (GLint*)values);
}

void Shader::setInt(const std::string& name, const int value) const
{
	glUniform1i(getUniformLocation(name), value);
}

void Shader::setIntArray(const std::string& name, GLsizei count, const GLint value[]) const
{
	glUniform1iv(getUniformLocation(name), count, value);
}

void Shader::setFloat(const std::string& name, const float value) const
{
	glUniform1f(getUniformLocation(name), value);
}

void Shader::setFloatArray(const std::string& name, GLsizei count, const GLfloat value[]) const
{
	glUniform1fv(getUniformLocation(name), count, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2f(getUniformLocation(name), value.x, value.y);
}

void Shader::setVec2Array(const std::string& name, GLsizei count, const glm::vec2 values[]) const
{
	glUniform2fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setVec3Array(const std::string& name, GLsizei count, const glm::vec3 values[]) const
{
	glUniform3fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
	glUniform4f(getUniformLocation(name), value.x, value.y, value.z, value.w);
}

void Shader::setVec4Array(const std::string& name, GLsizei count, const glm::vec4 values[]) const
{
	glUniform4fv(getUniformLocation(name), count, &values[0].x);
}

void Shader::setMat2(const std::string& name, const glm::mat2& value) const
{
	glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat2Array(const std::string& name, GLsizei count, const glm::mat2 values[]) const
{
	glUniformMatrix2fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) const
{
	glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat3Array(const std::string& name, GLsizei count, const glm::mat3 values[]) const
{
	glUniformMatrix3fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMat4Array(const std::string& name, GLsizei count, const glm::mat4 values[]) const
{
	glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, &values[0][0].x);
}

void Shader::bindShaderUboToBindingPoint(const std::string& uniformBlockName, const GLuint bindingPoint) const
{
	glUniformBlockBinding(shaderID, glGetUniformBlockIndex(shaderID, uniformBlockName.c_str()), bindingPoint);
}
