//TODO - Change to GLFW

#include <Windows.h>
#include <iostream>
#include <time.h>
#include <common/Shader.h>
#include <common/Defines.h>
#include <vector>
#include <GLM.h>
#include "glm/ext.hpp"
#include <common/Sprite.h>
#include <common/Renderer.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
/*----------------------------------------------------------------------------
						GLOBAL VARIABLES AND CONSTANTS
----------------------------------------------------------------------------*/

const int width = 900, height = 900;
const int gMaxBullets = 10;
const int gMaxEnemiesPerLine = 10;
const int gMaxLines = 6;

/*----------------------------------------------------------------------------
								STRUCTS
----------------------------------------------------------------------------*/

struct Player 
{
	int moving = 0;
	bool shooting = false;
	float speed;
	float cooldownTime;
	Sprite sprite;
	Player() : 
		speed(0.01f),
		cooldownTime(1.0f)
	{
		sprite.loadSprite("../Textures/brickwall.jpg");
		//Now we need to place the player at the bottom of the screen
		float scale = sprite.textureDimensions.y / (float)height;
		sprite.setPosition({ 0.0f, -1.0f + scale});
	}
};

struct Bullet
{
	float speed;
	bool enabled;
	Sprite sprite;
	Bullet() :
		speed(1.5f),
		enabled(false)
	{
		sprite.loadSprite("../Textures/brickwall.jpg");
	}
};

struct Enemy 
{
	glm::vec2 pos;
	Sprite sprite;
	Enemy() :
		pos({ 0.0f, 0.0f }) 
	{
		sprite.loadSprite("../Textures/alien.png");
	}
};

struct GameState
{
	Player player;
	std::vector<Bullet*> bullets;
	unsigned int bIndex;
	std::vector<Enemy*> enemies;
	unsigned int eIndex;

	float gameTime;

	float enemySpeed;

	GameState() :
		enemySpeed(0.01f),
		gameTime(0.0f),
		eIndex(0),
		bIndex(0)
	{
		for (int i = 0; i < gMaxEnemiesPerLine * gMaxLines; i++)
		{
			Enemy* enemy = new Enemy();
			enemies.push_back(enemy);
		}
		for (int i = 0; i < gMaxBullets; i++)
		{
			Bullet* bullet = new Bullet();
			bullets.push_back(bullet);
		}
	}

	~GameState()
	{
		for (unsigned int i = 0; i < bullets.size(); i++)
		{
			delete bullets[i];
		}
		for (unsigned int i = 0; i < enemies.size(); i++)
		{
			delete enemies[i];
		}
	}

};

/*----------------------------------------------------------------------------
						MESH AND TEXTURE VARIABLES
----------------------------------------------------------------------------*/

float points[] = {
	0.0f,  0.5f,  0.0f,
	0.5f, -0.5f,  0.0f,
	-0.5f, -0.5f,  0.0f
};

/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/
GLuint simpleShaderID;
/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/

const char* atlas_image = "../freemono.png";
const char* atlas_meta = "../freemono.meta";

GameState* gameState;
GLuint vbo;
GLuint vao;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

void endGame(int errCode)
{
	delete gameState;
	std::cin.get();
	exit(errCode);
}

void UpdateAndRenderPlayer(const float& dt)
{
	//Setting Player Movement
	Sprite* sprite = &gameState->player.sprite;
	float relWidth = sprite->textureDimensions.x / (float)width;
	float pos = sprite->getPosition().x + gameState->player.moving * gameState->player.speed;
	if (pos > 1.0f - relWidth)
		pos = 1.0f - relWidth;
	else if (pos < -1.0f + relWidth)
		pos = -1.0f + relWidth;
	sprite->setPosition({ pos, sprite->getPosition().y});

	static float cooldownTimer = 0;
	cooldownTimer -= dt;
	if (cooldownTimer < 0)
		cooldownTimer = 0;
	//Creating bullets that the player is firing
	if (gameState->player.shooting && cooldownTimer == 0)
	{
		cooldownTimer = gameState->player.cooldownTime;
		gameState->player.shooting = false;

		Bullet* bullet = gameState->bullets[gameState->bIndex];
		bullet->enabled = true;
		bullet->sprite.setPosition(gameState->player.sprite.getPosition() + glm::vec2(0.0f, 0.01f));

		if (gameState->bIndex + 1 >= (int)gameState->bullets.size())
			gameState->bIndex = 0;
		else
			gameState->bIndex++;
		
	}

	Renderer* renderer = Renderer::getInstance();
	RenderVariables variables;
	glm::vec2 scale = { sprite->textureDimensions.x / 900.0f, sprite->textureDimensions.y / 900.0f };
	//variables.model = glm::translate(variables.model, glm::vec3(sprite->getPosition(), 0.0));
	variables.model = glm::scale(variables.model, glm::vec3(0.00001, 0.00001, 1.0));

	renderer->Render(vao, 3, variables, simpleShaderID);
}
void UpdateAndRenderEnemies(const float& dt)
{
	//Rendering Loop for Enemies
	for (unsigned int i = 0; i < gameState->enemies.size(); i++)
	{

	}
}
void UpdateAndRenderBullets(const float& dt)
{
	//Rendering Loop for Bullets
	for (std::vector<Bullet*>::iterator it = gameState->bullets.begin(); it != gameState->bullets.end(); )
	{
		Bullet* bullet = *it;
		if (!bullet->enabled)
		{
			++it;
			continue;
		}
		bullet->sprite.setPosition(bullet->sprite.getPosition() + glm::vec2(0.0, bullet->speed*dt));
		if (bullet->sprite.getPosition().y > 1.0f)
		{
			bullet->enabled = false;
			printf("Bullet deactivated\n");
		}
		//TODO - Implement Collision Detection between the bullets and the enemies.

		//TODO - Render any bullets currently on-screen.

		Renderer* renderer = Renderer::getInstance();
		RenderVariables variables;
		Sprite* sprite = &bullet->sprite;

		//bullet->pos = glm::vec2(0.0, 0.001);

		glm::vec2 scale = { sprite->textureDimensions.x / (float)width, sprite->textureDimensions.y / (float)height };
		variables.model = glm::translate(variables.model, glm::vec3(sprite->getPosition(), 0.0));
		variables.model = glm::scale(variables.model, glm::vec3(scale.x, scale.y, 0.0));

		renderer->Render(Sprite::getVAO(), 6, variables, simpleShaderID, sprite->tex);
		printf("Rendering Bullet...");

		++it;
	}

}
void UpdateAndRenderShields(const float& dt)
{

}

/*--------------------------------------------------------------------------*/

void init()
{
	std::string file_path = __FILE__;
	std::string dir_path = file_path.substr(0, file_path.rfind("\\"));
	std::cout << dir_path << std::endl;

	Shader* factory = Shader::getInstance();		//Grab the static Shader instance from memory
	std::string log;
	if ((simpleShaderID = factory->CompileShader(SIMPLE_VERT, SIMPLE_FRAG, log)) < 0)
	{
		std::cout << "Log:" << log << "\n" << std::endl;
		endGame(-5);
	}
	Shader::resetInstance();						//Delete the Shader Instance once you are finished with it.

	//GameState Setup

}

void display()
{
	//Setup of the time difference between frames
	static clock_t lastFrame = clock();
	clock_t currFrame = clock();
	float delta = (currFrame - lastFrame) / (float)CLOCKS_PER_SEC;
	if (delta >= 0.0166f)
	{
		delta = 0.0166f;	//Locks physics interactions to 60fps, should that be necessary
	}

	glEnable(GL_DEPTH_TEST);								// enable depth-testing
	glDepthFunc(GL_LESS);									// depth-testing interprets a smaller value as "closer"
	glEnable(GL_BLEND);										// enable blending to allow for Alpha value usage.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear the color and buffer bits to make a clean slate
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);					// Create a background

	gameState->gameTime += delta;

	//glUseProgram(simpleShaderID);
	//glBindVertexArray(vao);
	//// draw points 0-3 from the currently bound VAO with current in-use shader
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	UpdateAndRenderPlayer(delta);
	//UpdateAndRenderBullets(delta);
	UpdateAndRenderShields(delta);
	UpdateAndRenderEnemies(delta);
}

#pragma region INPUT FUNCTIONS

void pollInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		gameState->player.moving = 1;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		gameState->player.moving = -1;
	else
		gameState->player.moving = 0;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		gameState->player.shooting = true;
	else
		gameState->player.shooting = false;
}
#pragma endregion INPUT FUNCTIONS

void main(int argc, char** argv) 
{
	if (!glfwInit())
	{
		exit(-1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Space Invaders", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		endGame(-1);
	}
	glfwMakeContextCurrent(window);
	glViewport(0, 0, width, height);

	// A call to glewInit() must be done after GLFW is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		endGame(1);
	}

	gameState = new GameState();
	
	init();

	vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

	vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	while (!glfwWindowShouldClose(window))
	{
		pollInput(window);
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	endGame(0);
}