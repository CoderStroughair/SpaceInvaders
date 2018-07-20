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
	bool AddShader(const char* inShaderLocation, GLenum inShaderType, GLuint inShaderProgram);
	char* readShaderSource(const char* shaderFile, bool& success);
	bool checkCompileError(GLuint shader, GLenum ShaderType);
	bool checkLinkError(GLuint shader);
	bool checkValidationErrors(GLuint program);
	static bool CheckStatus(GLuint obj);
	static Shader* Shader::instance;

public:
	bool CompileShader(GLuint& inOutShaderProgram, char* vertex, char* fragment);
	GLuint CompileComputeShader(char* compute);
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