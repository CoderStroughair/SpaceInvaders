#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <stdio.h>
#include <string>

/**
Requirements:
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
In your Init() Function:
shaderProgramID = basic.CompileShader(vert, frag);

If using Compute Shaders, call CompileComputeShader(compute) instead.
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
Then do all your regular mumbo jumbo inside your display file (linking uniforms and such)
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

This class was originally created by following the tutorial steps outlined on learnopengl.com
**/
class Shader 
{
private:
	Shader() {};
	bool AddShader(GLuint &ShaderProgram, const char* pShaderText, GLenum ShaderType, std::string& errLog);
	char* readShaderSource(const char* shaderFile, bool& success, std::string& errLog);
	bool checkCompileError(GLuint shader, GLenum ShaderType, std::string& errLog);
	bool checkLinkError(GLuint shader, std::string& errLog);
	bool checkValidationErrors(GLuint program, std::string& errLog);
	static bool CheckStatus(GLuint obj, std::string& errLog);
	static Shader* Shader::instance;

public:
	GLuint CompileShader(char* vertex, char* fragment, std::string& errLog);
	GLuint CompileComputeShader(char* compute, std::string& errLog);
	static Shader* getInstance()
	{
		if (Shader::instance == NULL)
		{
			Shader::instance = new Shader();
		}
		return Shader::instance;
	}
	static void resetInstance()
	{
		delete Shader::instance;
		instance = NULL;
	}

};
#endif