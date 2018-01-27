#ifndef RENDERER_H
#define RENDERER_H

#include <string>
#include <vector>
#include <GLM.h>

enum VarType
{
	integer,
	floating,
	vec2,
	vec3,
	vec4,
	mat3,
	mat4
};

struct LightStruct {

	// light properties
	glm::vec3 Ls = glm::vec3(1.0f, 1.0f, 1.0f);			//Specular Reflected Light
	glm::vec3 Ld = glm::vec3(0.5f, 0.5f, 0.5f);			//Diffuse Surface Reflectance
	glm::vec3 La = glm::vec3(0.1f, 0.1f, 0.1f);			//Ambient Reflected Light
	glm::vec3 lightLocation = glm::vec3(0, 10, -5.0f);	//light source location
	glm::vec3 coneDirection = lightLocation + glm::vec3(0.0f, -1.0f, 0.0f);
	float coneAngle = 40.0f;

	// object colour
	glm::vec3 Ks = glm::vec3(0.001f, 0.001f, 0.001f);	// specular reflectance
	glm::vec3 Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 Ka = glm::vec3(1.0f, 1.0f, 1.0f);			// ambient reflectance
	float specular_exponent = 0.5f;						//specular power

};

struct ShaderUniform	//Allows for a universal rendering function, regardless of uniforms
{
	std::string name;
	VarType varType;
	int integer;
	float floating;
	glm::vec2 vec2;
	glm::vec3 vec3;
	glm::vec4 vec4;
	glm::mat3 mat3;
	glm::mat4 mat4;
};

struct RenderVariables
{
	std::vector<ShaderUniform> uniforms;
	glm::mat4 model;
	glm::mat4 proj;
	glm::mat4 view;

	RenderVariables() :
		model(glm::mat4()),
		proj(glm::mat4()),
		view(glm::mat4()) {}
};

class Renderer
{

private:
	Renderer() {};
	static Renderer* Renderer::instance;

public:
	static Renderer* getInstance()
	{
		if (Renderer::instance == NULL)
		{
			printf("Initialising Renderer\n");
			Renderer::instance = new Renderer();
		}
		return Renderer::instance;
	}
	static void resetInstance()
	{
		delete Renderer::instance;
		instance = NULL;
	}

	void Render(const unsigned int& VAO, const unsigned int& numVertices, const RenderVariables& variables, const unsigned int& shaderID, const unsigned int& tex = 0);


};

#endif