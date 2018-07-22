#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <GLM.h>
#include <GL/glew.h>

class Sprite
{
private:

	glm::vec3 pos;	//z coordinate to be used as layering tool
	float rotation;	//Only considering rotation on Z axis, since this is a 2D tool.
	float scale;


	static unsigned int VAO;
	static void generateVAO()	//Singleton VAO
	{
		if(VAO)
			glDeleteVertexArrays(1, &VAO);	//clearing any previously created VAO
		unsigned int VBO = 0;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		float* verts = new float[24]
		{
			//positions		//textures
			-1.0f, -1.0f,	0.0f, 0.0f,	//Bottom Left
			-1.0f,  1.0f,	0.0f, 1.0f,	//Top Left
			 1.0f, -1.0f,	1.0f, 0.0f,	//Bottom Right
			
			 1.0f,  1.0f,	1.0f, 1.0f,	//Top Right
			-1.0f,  1.0f,	0.0f, 1.0f,	//Top Left
			 1.0f, -1.0f,	1.0f, 0.0f	//Bottom Right
		};

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), verts, GL_STATIC_DRAW); //allocating space, and inputting data

		glEnableVertexAttribArray(0);
		GLintptr stride = 4 * sizeof(float);	//distance between vertex/texture coord starting points in memory
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);	//adding markers to find vertex data

		glEnableVertexAttribArray(1);
		GLintptr offset = 2 * sizeof(float);	//used at start to locate first texture coordinate
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)offset);	//adding markers to find texture data
	}
public:
	Sprite()
	{
		pos = glm::vec3(0);
		rotation = 0;
		scale = 1;
	}
	bool loadSprite(char* texLocation);
	void setPosition(const glm::vec2& _pos);
	glm::vec2 getPosition();
	void setLayer(const float& layer);
	float getLayer();
	void rotate(const float& radians);
	float getRotation();
	static unsigned int getVAO()
	{
		if (VAO == 0)
		{
			printf("Initialising VAO\n");
			generateVAO();
		}
		return VAO;
	}
	unsigned int tex;

	glm::ivec2 textureDimensions = glm::vec2();
};

#endif
