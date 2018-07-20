#include <common/Shader.h>
#include <Windows.h>

#include <iostream>

#define _CRT_SECURE_NO_WARNINGS

Shader* Shader::instance = NULL;

char* Shader::readShaderSource(const char* shaderFile, bool& success)
{
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL)
	{
		printf_s("Error: Shader %s not found", shaderFile);
		success = false;
		return "";
	}

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	success = true;
	return buf;
}

bool Shader::AddShader(const char* inShaderLocation, GLenum inShaderType, GLuint inShaderProgram)
{
	GLuint shader = glCreateShader(inShaderType);
	bool success = false;
	const char* shaderText = readShaderSource(inShaderLocation, success);
	if (!success)
	{
		return false;
	}
	glShaderSource(shader, 1, &shaderText, NULL);
	glCompileShader(shader);
	if (!CheckStatus(shader))
	{
		return false;
	}
	glAttachShader(inShaderProgram, shader);
	return true;
}

bool Shader::CompileShader(GLuint& inOutShaderProgram, char* inVertexLocation, char* inFragmentLocation)
{
	inOutShaderProgram = glCreateProgram();
	if (!AddShader(inVertexLocation, GL_VERTEX_SHADER, inOutShaderProgram) || !AddShader(inFragmentLocation, GL_FRAGMENT_SHADER, inOutShaderProgram))
	{
		return false;
	}
	glLinkProgram(inOutShaderProgram);
	if (!CheckStatus(inOutShaderProgram))
	{
		return false;
	}
	return true;
}

GLuint Shader::CompileComputeShader(char* compute)
{
	GLuint shaderProgramID = glCreateProgram();
	/*if (shaderProgramID == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(104);
	}
	if (!AddShader(shaderProgramID, compute, GL_COMPUTE_SHADER)) { return -1; }

	glLinkProgram(shaderProgramID);
	if(!checkLinkError(shaderProgramID)) { return -1; }*/

	//glValidateProgram(shaderProgramID);
	//if (!checkValidationErrors(shaderProgramID)) { return -1; }
	return shaderProgramID;
}

bool Shader::checkCompileError(GLuint shader, GLenum ShaderType)
{
	GLint params = -1;
	GLchar ErrorLog[512] = { 0 };
	GLint size = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &params);
		glGetShaderInfoLog(shader, 512, &size, ErrorLog);
		printf("Error compiling shader number %i of type %d: '%s'\n", shader, ShaderType, ErrorLog);
		return false;
	}
	return true;
}

bool Shader::checkLinkError(GLuint program) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		printf("Linking Error: Invalid shader program: '%s'\n", ErrorLog);
		return false;
	}
	return true;
}

bool Shader::checkValidationErrors(GLuint program) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	if (GL_TRUE != params)
	{
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		printf("Validation Error: Invalid shader program: '%s'\n", ErrorLog);
		return false;
	}
	return true;
}

bool Shader::CheckStatus(GLuint obj)
{
	GLint status = GL_FALSE;
	if (glIsShader(obj))
	{
		glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	}
	else if (glIsProgram(obj))
	{
		glGetProgramiv(obj, GL_LINK_STATUS, &status);
	}
	if (status == GL_TRUE) return true;
	GLchar log[1 << 15] = { 0 };
	if (glIsShader(obj))
	{
		GLint param;
		glGetShaderiv(obj, GL_SHADER_TYPE, &param);
		glGetShaderInfoLog(obj, sizeof(log), NULL, log);
		printf("There is a problem in shader %d", param);
	}
	else if (glIsProgram(obj))
	{
		glGetProgramInfoLog(obj, sizeof(log), NULL, log);
		printf("There is a problem in this program: ");
	}
	printf(log);
	return false;
}