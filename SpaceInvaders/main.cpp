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
#include <common/Framebuffer.h>
#include <GLFW/glfw3.h>

/*----------------------------------------------------------------------------
						GLOBAL VARIABLES AND CONSTANTS
----------------------------------------------------------------------------*/

const int width = 1200, height = 1200;
const int gMaxBullets = 5;
const int gMaxEnemiesPerLine = 6;
const int gMaxLines = 6;

DWORD lastFrame;

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
		speed(0.0005f),
		cooldownTime(0.5f)
	{
		sprite.loadSprite("../Textures/alien.png");
		//Now we need to place the player at the bottom of the screen
		float scale = sprite.textureDimensions.y / (float)height;
		sprite.setPosition({ 0.0f, -1.0f + scale});
	}
};

struct Bullet
{
	float speed;
	Sprite sprite;
	Bullet() :
		speed(1.0f)
	{
		sprite.loadSprite("../Textures/bullet.png");
	}
};

struct Enemy 
{
	Sprite sprite;
	float speed;
	bool enabled;
	Enemy() :
		speed(1.0f),
		enabled(false)
	{
		sprite.loadSprite("../Textures/alien.png");
		sprite.setPosition(glm::vec2(0.0, 0.0));
	}
};

struct GameState
{
	Player player;
	std::vector<Bullet*> bullets;
	std::vector<Bullet*>::iterator bulletPointer;
	std::vector<Enemy*> enemies;
	std::vector<Enemy*>::iterator enemyPointer;

	float gameTime;

	float enemySpeed;

	GameState() :
		enemySpeed(0.01f),
		gameTime(0.0f)
	{
		for (int i = 0; i < gMaxEnemiesPerLine * gMaxLines; i++)
		{
			Enemy* enemy = new Enemy();
			enemy->enabled = true;
			enemies.push_back(enemy);
		}
		enemyPointer = enemies.begin();
		for (int i = 0; i < gMaxBullets; i++)
		{
			Bullet* bullet = new Bullet();
			bullets.push_back(bullet);
		}
		bulletPointer = bullets.begin();
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
GLuint shader_programme;
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
	printf("Error Code %d", errCode);
	if (errCode != 0)
	{
		std::cin.get();
	}
	delete gameState;
	exit(errCode);
}

void SetupEnemies()
{
	float scale = gameState->enemies[0]->sprite.textureDimensions.y / (float)height;
	glm::vec2 initPosition = glm::vec2({scale - 1, 1 - scale });
	glm::vec2 currPosition = initPosition;
	int numEnemiesOnLine = 0;
	int currLine = 0;
	for (std::vector<Enemy*>::iterator it = gameState->enemies.begin(); it != gameState->enemies.end(); )
	{
		Enemy* enemy = *it;
		enemy->sprite.setPosition({ currPosition.x, currPosition.y });
		numEnemiesOnLine++;
		if (numEnemiesOnLine == gMaxEnemiesPerLine)
		{
			numEnemiesOnLine = 0;
			currLine++;
			currPosition = initPosition + glm::vec2(0.0, -scale * 2 * currLine);
		}
		else
		{
			currPosition = currPosition + glm::vec2(scale * 2, 0.0f);
		}
		gameState->enemyPointer++;
		it++;
	}
}

void UpdatePlayerPhysics(const float& dt)
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
	cooldownTimer += dt;
	//Creating bullets that the player is firing
	if ((gameState->player.shooting) && (cooldownTimer > gameState->player.cooldownTime) && (gameState->bulletPointer != gameState->bullets.end()) )
	{
		cooldownTimer = 0;
		gameState->player.shooting = false;

		Bullet* bullet = *gameState->bulletPointer;
		bullet->sprite.setPosition(glm::vec2(gameState->player.sprite.getPosition().x, gameState->player.sprite.getPosition().y +(sprite->textureDimensions.y / (float)height) / 2));
		gameState->bulletPointer++;		
	}
}
void UpdateEnemyPhysics(const float& dt)
{
	//Rendering Loop for Enemies
	for (std::vector<Enemy*>::iterator it = gameState->enemies.begin(); it < gameState->enemyPointer; )
	{
		++it;
	}
}
void UpdateBulletPhysics(const float& dt)
{
	//Rendering Loop for Bullets
	for (std::vector<Bullet*>::iterator it = gameState->bullets.begin(); it < gameState->bulletPointer; )
	{
		Bullet* bullet = *it;
		bullet->sprite.setPosition(bullet->sprite.getPosition() + glm::vec2(0.0, bullet->speed*dt));
		if (bullet->sprite.getPosition().y > 1.0f)
		{
			std::iter_swap(it, gameState->bulletPointer - 1);
			gameState->bulletPointer--;
		}
		//TODO - Implement Collision Detection between the bullets and the enemies.
		++it;
	}
}
void UpdateShieldPhysics(const float& dt)
{

}

void RenderScene()
{
	Renderer* renderer = Renderer::getInstance();
	RenderVariables renderVariables;
	Sprite* sprite;
	//Render the Player
	{
		sprite = &gameState->player.sprite;
		glm::vec2 scale = { sprite->textureDimensions.x / (float)width, sprite->textureDimensions.y / (float)height };
		renderVariables.model = glm::translate(renderVariables.model, glm::vec3(sprite->getPosition(), 0.0));
		renderVariables.model = glm::scale(renderVariables.model, glm::vec3(scale.x, scale.y, 0.0));
		renderer->Render(Sprite::getVAO(), 6, renderVariables, simpleShaderID, sprite->tex);
	}

	//Render the Bullets
	{
		for (std::vector<Bullet*>::iterator it = gameState->bullets.begin(); it != gameState->bulletPointer; )
		{
			renderVariables = {};
			Bullet* bullet = *it;
			sprite = &bullet->sprite;
			glm::vec2 scale = { sprite->textureDimensions.x / ((float)width), sprite->textureDimensions.y / ((float)height) };
			renderVariables.model = glm::translate(renderVariables.model, glm::vec3(sprite->getPosition(), 0.0));
			renderVariables.model = glm::scale(renderVariables.model, glm::vec3(scale.x, scale.y, 0.0));
			renderer->Render(Sprite::getVAO(), 6, renderVariables, simpleShaderID, sprite->tex);
			++it;
		}
	}

	//Render the Enemies
	{
		for (std::vector<Enemy*>::iterator it = gameState->enemies.begin(); it != gameState->enemyPointer; )
		{
			renderVariables = {};
			Enemy* bullet = *it;
			sprite = &bullet->sprite;
			glm::vec2 scale = { sprite->textureDimensions.x / ((float)width), sprite->textureDimensions.y / ((float)height) };
			renderVariables.model = glm::translate(renderVariables.model, glm::vec3(sprite->getPosition(), 0.0));
			renderVariables.model = glm::scale(renderVariables.model, glm::vec3(scale.x, scale.y, 0.0));
			renderer->Render(Sprite::getVAO(), 6, renderVariables, simpleShaderID, sprite->tex);
			++it;
		}
		printf("\n");
	}

	//Render the Shields
	{

	}
}

/*--------------------------------------------------------------------------*/

long long milliseconds_now() {
	//TODO - Setup system for cross-platform compatibility

	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

void init()
{
	Shader* factory = Shader::getInstance();		//Grab the static Shader instance from memory
	;
	if (!factory->CompileShader(simpleShaderID, SIMPLE_VERT, SIMPLE_FRAG))
	{
		endGame(5);
	}
	Shader::resetInstance();						//Delete the Shader Instance once you are finished with it.

	//GameState Setup

}

void display()
{
	DWORD currFrame = milliseconds_now();
	float delta = (currFrame - lastFrame)/1000.0f;
	if (delta == 0)
	{
		//return;
	}
	lastFrame = currFrame;

	glEnable(GL_DEPTH_TEST);								// enable depth-testing
	glDepthFunc(GL_LESS);									// depth-testing interprets a smaller value as "closer"
	glEnable(GL_BLEND);										// enable blending to allow for Alpha value usage.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear the color and buffer bits to make a clean slate
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);					// Create a background

	glUseProgram(simpleShaderID);
	//glBindVertexArray(vao);
	//draw points 0-3 from the currently bound VAO with current in-use shader
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	gameState->gameTime += delta;

	UpdatePlayerPhysics(delta);
	UpdateBulletPhysics(delta);
	UpdateShieldPhysics(delta);
	UpdateEnemyPhysics(delta);

	RenderScene();
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

	std::string log;
	Framebuffer frameBuffer;
	frameBuffer.init(width, height, log);
	lastFrame = milliseconds_now();
	SetupEnemies();
	while (!glfwWindowShouldClose(window))
	{
		pollInput(window);
		display();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	endGame(0);
}