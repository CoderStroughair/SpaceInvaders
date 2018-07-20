#include <common/Shader.h>
#include <Windows.h>

#include <iostream>

Shader* Shader::instance = NULL;

char* Shader::readShaderSource(const char* shaderFile, bool& success, std::string& errLog)
{
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");
	if (fp == NULL)
	{
		errLog = "Error: Shader %s not found", shaderFile;
		return false;
	}

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}

bool Shader::AddShader(GLuint &shaderProgram, const char* pShaderText, GLenum shaderType, std::string& errLog)
{
	GLuint shaderObj = glCreateShader(shaderType);
	if (shaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", shaderType);
		exit(102);
	}
	bool success;
	const char* pShaderSource = readShaderSource(pShaderText, success, errLog);
	printf(pShaderSource);
	if (!success)
	{
		return false;
	}
	glShaderSource(shaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	glCompileShader(shaderObj);
	CheckStatus(shaderObj, errLog);
	if (!checkCompileError(shaderObj, shaderType, errLog))
	{
		return false;
	}
	glAttachShader(shaderProgram, shaderObj);
	return true;
}

GLuint Shader::CompileShader(char* vertex, char* fragment, std::string& errLog)
{
	GLuint shaderProgramID = glCreateProgram();
	if (!shaderProgramID)
	{
		errLog = "Error creating shader program\n";
		return -1;
	}
	if(!AddShader(shaderProgramID, vertex, GL_VERTEX_SHADER, errLog)){return -1;}

	if(!AddShader(shaderProgramID, fragment, GL_FRAGMENT_SHADER, errLog)){return -1;}

	glLinkProgram(shaderProgramID);
	if (!checkLinkError(shaderProgramID, errLog))
	{
		return -1;
	}

	glValidateProgram(shaderProgramID);
	if (!checkValidationErrors(shaderProgramID, errLog))
	{
		return -1;
	}

	return shaderProgramID;
}

GLuint Shader::CompileComputeShader(char* compute, std::string& errLog)
{
	GLuint shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0)
	{
		fprintf(stderr, "Error creating shader program\n");
		exit(104);
	}
	if (!AddShader(shaderProgramID, compute, GL_COMPUTE_SHADER, errLog)) { return -1; }

	glLinkProgram(shaderProgramID);
	if(!checkLinkError(shaderProgramID, errLog)) { return -1; }

	glValidateProgram(shaderProgramID);
	if (!checkValidationErrors(shaderProgramID, errLog)) { return -1; }
	return shaderProgramID;
}

bool Shader::checkCompileError(GLuint shader, GLenum ShaderType, std::string& errLog)
{
	GLint params = -1;
	GLchar ErrorLog[512] = { 0 };
	GLint size = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &params);
		glGetShaderInfoLog(shader, 512, &size, ErrorLog);
		char error[700] = { 0 };
		sprintf_s(error, "Error compiling shader number %i of type %d: '%s'\n", shader, ShaderType, ErrorLog);
		errLog = std::string(error);
		return false;
	}
	return true;
}

bool Shader::checkLinkError(GLuint program, std::string& errLog) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		errLog = "Linking Error: Invalid shader program: '%s'\n", ErrorLog;
		return false;
	}
	return true;
}

bool Shader::checkValidationErrors(GLuint program, std::string& errLog) {
	GLint params = -1;
	GLchar ErrorLog[1024] = { 0 };
	glGetProgramiv(program, GL_VALIDATE_STATUS, &params);
	if (GL_TRUE != params)
	{
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		errLog = ("Validation Error: Invalid shader program: '%s'\n", ErrorLog);
		return false;
	}
	return true;
}

bool Shader::CheckStatus(GLuint obj, std::string& errLog)
{
	GLint status = GL_FALSE;
	if (glIsShader(obj))
	{
		glGetShaderiv(obj, GL_COMPILE_STATUS, &status);
	}
	if (glIsProgram(obj))
	{	
		glGetProgramiv(obj, GL_LINK_STATUS, &status);
	}
	if (status == GL_TRUE)
	{
		return true;
	}
	GLchar log[1 << 15] = { 0 };
	if (glIsShader(obj))
	{
		glGetShaderInfoLog(obj, sizeof(log), NULL, log);
	}
	else if (glIsProgram(obj))
	{
		glGetProgramInfoLog(obj, sizeof(log), NULL, log);
	}
	errLog = log;
	printf(log);
	return false;
}