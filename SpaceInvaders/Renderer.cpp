#include "Renderer.h"

#include <GL\glew.h>
#include <Windows.h>

Renderer* Renderer::instance = NULL;

void Renderer::Render(const unsigned int& VAO, const unsigned int& numVertices, const RenderVariables& variables, const unsigned int& shaderID, const unsigned int& tex)
{

	glUseProgram(shaderID);
	glBindVertexArray(VAO);

	glUniformMatrix4fv(glGetUniformLocation(shaderID, "view"), 1, GL_FALSE, &variables.view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &variables.proj[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &variables.model[0][0]);

	for each (ShaderUniform uniform in variables.uniforms)
	{
		printf("Sending Uniforms\n");
		unsigned int location = glGetUniformLocation(shaderID, uniform.name.c_str());
		if (location == -1)
		{
			std::string errLog = "ERROR: uniform not found - ";
			errLog += uniform.name.c_str();
			OutputDebugString(errLog.c_str());
		}
		switch (uniform.varType)
		{
		case integer:
			glUniform1i(location, uniform.integer);
			break;
		case floating:
			glUniform1f(location, uniform.floating);
			break;
		case vec2:
			glUniform2fv(location, 1, &uniform.vec2[0]);
			break;
		case vec3:
			glUniform3fv(location, 1, &uniform.vec3[0]);
			break;
		case vec4:
			glUniform4fv(location, 1, &uniform.vec4[0]);
			break;
		case mat3:
			glUniformMatrix3fv(location, 1, GL_FALSE, &uniform.mat3[0][0]);
			break;
		case mat4:
			glUniformMatrix4fv(location, 1, GL_FALSE, &uniform.mat4[0][0]);
			break;
		default:
			std::string errorLog = "ERROR: could not load uniform - ";
			errorLog += uniform.name;
			OutputDebugString(errorLog.c_str());
			break;
		}
	}

	if (tex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glUniform1i(glGetUniformLocation(shaderID, "texture_primary"), 0);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);
}