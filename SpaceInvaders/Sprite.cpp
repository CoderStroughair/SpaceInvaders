#include "Sprite.h"

//uncomment if needed
//#define STB_IMAGE_IMPLEMENTATION
#include <common/stb_image.h>
#include <string>
#include <Windows.h>

unsigned int Sprite::VAO = 0;

bool Sprite::loadSprite(char* texLocation)
{
	int n;
	int force_channels = 4;
	unsigned char* image_data = stbi_load(texLocation, &textureDimensions.x, &textureDimensions.y, &n, force_channels);
	if (!image_data)
	{
		std::string errorLog = "ERROR: could not load ";
		errorLog += texLocation;
		errorLog += "\n";
		fprintf(stderr, errorLog.c_str());
		OutputDebugString(errorLog.c_str());
		return false;
	}
	// NPOT check
	if ((textureDimensions.x & (textureDimensions.x - 1)) != 0 || (textureDimensions.y & (textureDimensions.y - 1)) != 0)
	{
		std::string warning = "WARNING: texture ";
		warning += texLocation;
		warning += " is not power-of-2 dimensions\n";
		fprintf(stderr, warning.c_str());
		OutputDebugString(warning.c_str());
	}
	int width_in_bytes = textureDimensions.x * 4;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = textureDimensions.y / 2;

	for (int row = 0; row < half_height; row++)
	{
		top = image_data + row * width_in_bytes;
		bottom = image_data + (textureDimensions.y - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}
	glGenTextures(1, &tex);
	//glActiveTexture(mode);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureDimensions.x, textureDimensions.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);

	delete image_data;
	return true;
}

void Sprite::setPosition(const glm::vec2& _pos)
{
	pos.x = _pos.x;
	pos.y = _pos.y;
}

glm::vec2 Sprite::getPosition()
{
	return {pos.x, pos.y};
}

void inline Sprite::setLayer(const float& layer)
{
	pos.z = layer;
}

float inline Sprite::getLayer() 
{
	return pos.z;
}

void inline Sprite::rotate(const float& radians)
{
	static float max = 2 * glm::pi<float>();	//mitigates underflow/overflow by keeping range between -2pi and +2pi

	rotation += radians;
	while (rotation > max)
		rotation -= max;
	while (rotation < -max)
		rotation += max;
}

float inline Sprite::getRotation()
{
	return rotation;
}


