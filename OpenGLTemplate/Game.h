#pragma once

#include "Common.h"
#include "GameWindow.h"

// Classes used in game.  For a new class, declare it here and provide a pointer to an object of this class below.  Then, in Game.cpp, 
// include the header.  In the Game constructor, set the pointer to NULL and in Game::Initialise, create a new object.  Don't forget to 
// delete the object in the destructor.   
class CCamera;
class CSkybox;
class CShader;
class CShaderProgram;
class CPlane;
class CFreeTypeFont;
class CHighResolutionTimer;
class CSphere;
class COpenAssetImportMesh;

class CCatmullRom;
class CPlayer;
class CCoin;
class CLamp;
class CCone;

class Game {
private:
	// Three main methods used in the game.  Initialise runs once, while Update and Render run repeatedly in the game loop.
	void Initialise();
	void Update();
	void Render();

	// Pointers to game objects.  They will get allocated in Game::Initialise()
	CSkybox *m_pSkybox;
	CCamera *m_pCamera;
	vector <CShaderProgram *> *m_pShaderPrograms;
	CPlane *m_pPlanarTerrain;
	CFreeTypeFont* m_scoreText;
	CFreeTypeFont* m_lapText;
	CFreeTypeFont* m_speedText;
	CFreeTypeFont* m_gameOverText;
	CSphere *m_pSphere;
	CHighResolutionTimer *m_pHighResolutionTimer;

	//Objects
	//Track
	CCatmullRom* m_track;
	CPlayer* m_player;
	std::vector<CCoin*> m_coins;
	int numberOfCoins;
	//Lamp
	CLamp* m_lamp;
	//Cone
	CCone* m_cone;
	std::vector<CCone*> m_cones;
	int numberOfCones;

	// Some other member variables
	double m_dt;
	int m_framesPerSecond;
	bool m_appActive;

	//Camera movement along path
	float m_currentDistance;
	glm::vec3 p;
	glm::vec3 pNext;

	//TNB frame
	glm::vec3 T;
	glm::vec3 up;
	glm::vec3 N;
	glm::vec3 B;

	//Camera position
	glm::vec3 m_cameraPosition;
	float m_cameraOffset;
	//Player rotation
	glm::mat4 m_carOrientation;
	//Current closest point;
	//Camera shake
	float m_jitterAngle =0.f;
	float m_jitterValue = 0.f;
	//Variable to switch between different views
	bool m_fpsView = false;

	//used as a timer for the shader
	float m_t;

	//Variable to check if we are in debug mode
	bool m_debug = false;

	//Lights
	int m_positions[20];
	glm::vec3 m_points[20];
	float m_angles[20];
	glm::mat4 m_matrices[20];
	int currentPos = 0;

	//Game over
	int m_lapCount = 1;
	bool m_gameOver = false;


public:
	Game();
	~Game();
	static Game& GetInstance();
	LRESULT ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param);
	void SetHinstance(HINSTANCE hinstance);
	WPARAM Execute();
	void CameraJitter();
	void ResetPlayer();
	void ResetCoins();
	void ResetCones();
	void SetLampOffsets();

private:
	static const int FPS = 60;
	void DisplayFrameRate();
	void GameLoop();
	GameWindow m_gameWindow;
	HINSTANCE m_hInstance;
	int m_frameCount;
	double m_elapsedTime;


};
