//TODO - Change to GLFW


#include <time.h>
#include <common/Shader.h>
#include <common/Defines.h>
#include <vector>
#include <GLM.h>
#include "glm/ext.hpp"
#include "Sprite.h"
#include "Renderer.h"
#include <common/text.h>

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
		sprite.loadSprite("../Textures/ship.png");

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
		speed(1.0f),
		enabled(false)
	{
		sprite.loadSprite("../Textures/bullet.png");
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
		enemySpeed(0.01),
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


/*----------------------------------------------------------------------------
								SHADER VARIABLES
----------------------------------------------------------------------------*/
GLuint simpleShaderID;
/*----------------------------------------------------------------------------
							OTHER VARIABLES
----------------------------------------------------------------------------*/

const char* atlas_image = "../freemono.png";
const char* atlas_meta = "../freemono.meta";

float fontSize = 25.0f;
int textID = -1;
int triangleDistance;

GameState* gameState;
/*----------------------------------------------------------------------------
						FUNCTION DEFINITIONS
----------------------------------------------------------------------------*/

void endGame(float errCode)
{
	delete gameState;
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
		bullet->sprite.setPosition(gameState->player.sprite.getPosition());

		if (gameState->bIndex + 1 >= (int)gameState->bullets.size())
			gameState->bIndex = 0;
		else
			gameState->bIndex++;
		
	}

	Renderer* renderer = Renderer::getInstance();
	RenderVariables variables;
	glm::vec2 scale = { sprite->textureDimensions.x / (float)width, sprite->textureDimensions.y / (float)height };
	variables.model = glm::translate(variables.model, glm::vec3(sprite->getPosition(), 0.0));
	variables.model = glm::scale(variables.model, glm::vec3(scale.x, scale.y, 0.0));

	renderer->Render(Sprite::getVAO(), 6, variables, simpleShaderID, sprite->tex);
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
		//printf("Rendering...");

		++it;
	}

}
void UpdateAndRenderShields(const float& dt)
{

}

/*--------------------------------------------------------------------------*/


void init()
{
	if (!init_text_rendering(atlas_image, atlas_meta, width, height)) 
	{
		fprintf(stderr, "ERROR init text rendering\n");
		OutputDebugString("Text Rendering Failed");
		endGame(2);
	}
	Shader* factory = Shader::getInstance();		//Grab the static Shader instance from memory
	simpleShaderID = factory->CompileShader(SIMPLE_VERT, SIMPLE_FRAG);
	Shader::resetInstance();						//Delete the Shader Instance once you are finished with it.

	//GameState Setup

}

void updateScene()
{
	glutPostRedisplay();
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);					// Create a background


	gameState->gameTime += delta;

	UpdateAndRenderPlayer(delta);
	UpdateAndRenderBullets(delta);
	UpdateAndRenderShields(delta);
	UpdateAndRenderEnemies(delta);

	glutSwapBuffers();
}


#pragma region INPUT FUNCTIONS

void keypress(unsigned char key, int x, int y) 
{
	if (key == (char)27)	//Pressing Escape Ends the game
	{
		exit(0);
	}

	if (key == 'd' || key == 'D')
	{
		gameState->player.moving = 1;
	}

	if (key == 'a' || key == 'A')
	{
		gameState->player.moving = -1;
	}

	if (key == ' ')
	{
		gameState->player.shooting = true;
	}
}

void keypressUp(unsigned char key, int x, int y)
{
	if (key == 'd' || key == 'D' || key == 'a' || key == 'A')
	{
		gameState->player.moving = 0;
	}
	if (key == ' ')
	{
		gameState->player.shooting = false;
	}
}

void specialKeypress(int key, int x, int y) 
{

}

void specialKeypressUp(int key, int x, int y) 
{

}

void (mouse)(int x, int y)
{

}

#pragma endregion INPUT FUNCTIONS

void main(int argc, char** argv) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("GameApp");
	glutSetCursor(GLUT_CURSOR_CROSSHAIR);


	// Tell glut where the display function is
	glutWarpPointer(width / 2, height / 2);
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);

	// Input Function Initialisers//
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouse);
	glutSpecialFunc(specialKeypress);
	glutSpecialUpFunc(specialKeypressUp);
	glutKeyboardUpFunc(keypressUp);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		endGame(1);
	}

	gameState = new GameState();
	


	init();
	textID = add_text("hi", -0.95f, 0.9f, fontSize, 1.0f, 1.0f, 1.0f, 1.0f);

	glutMainLoop();
	endGame(0);
}