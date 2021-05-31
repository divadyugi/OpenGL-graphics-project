/* 
OpenGL Template for INM376 / IN3005
City University London, School of Mathematics, Computer Science and Engineering
Source code drawn from a number of sources and examples, including contributions from
 - Ben Humphrey (gametutorials.com), Michal Bubner (mbsoftworks.sk), Christophe Riccio (glm.g-truc.net)
 - Christy Quinn, Sam Kellett and others

 For educational use by Department of Computer Science, City University London UK.

 This template contains a skybox, simple terrain, camera, lighting, shaders, texturing

 Potential ways to modify the code:  Add new geometry types, shaders, change the terrain, load new meshes, change the lighting, 
 different camera controls, different shaders, etc.
 
 Template version 5.0a 29/01/2017
 Dr Greg Slabaugh (gregory.slabaugh.1@city.ac.uk) 
*/


#include "game.h"


// Setup includes
#include "HighResolutionTimer.h"
#include "GameWindow.h"

// Game includes
#include "Camera.h"
#include "Skybox.h"
#include "Plane.h"
#include "Shaders.h"
#include "FreeTypeFont.h"
#include "Sphere.h"
#include "MatrixStack.h"
#include "OpenAssetImportMesh.h"

#include "CatmullRom.h"
#include "Player.h"
#include "Coin.h"
#include "Lamp.h"
#include "Cone.h"

// Constructor
Game::Game()
{
	m_pSkybox = NULL;
	m_pCamera = NULL;
	m_pShaderPrograms = NULL;
	m_pPlanarTerrain = NULL;
	m_scoreText = NULL;
	m_lapText = NULL;
	m_speedText = NULL;
	m_gameOverText = NULL;
	m_pSphere = NULL;
	m_pHighResolutionTimer = NULL;

	//Objects
	//Track
	m_track = NULL;
	//Player
	m_player = NULL;
	//Coins
	numberOfCoins = 10;
	//Lamp
	m_lamp = NULL;
	//Cone
	m_cone = NULL;
	numberOfCones = 10;

	m_dt = 0.0;
	m_framesPerSecond = 0;
	m_frameCount = 0;
	m_elapsedTime = 0.0f;
}

// Destructor
Game::~Game() 
{ 
	//game objects
	delete m_pCamera;
	delete m_pSkybox;
	delete m_pPlanarTerrain;
	delete m_scoreText;
	delete m_lapText;
	delete m_speedText;
	delete m_gameOverText;
	delete m_pSphere;

	//Objects
	delete m_track;
	delete m_player;
	for (auto coin : m_coins)
	{
		delete coin;
	}
	m_coins.clear();

	delete m_lamp;
	delete m_cone;
	for (auto cone : m_cones)
	{
		delete cone;
	}
	m_cones.clear();

	if (m_pShaderPrograms != NULL) {
		for (unsigned int i = 0; i < m_pShaderPrograms->size(); i++)
			delete (*m_pShaderPrograms)[i];
	}
	delete m_pShaderPrograms;

	//setup objects
	delete m_pHighResolutionTimer;
}

// Initialisation:  This method only runs once at startup
void Game::Initialise() 
{
	// Set the clear colour and depth
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	/// Create objects
	m_pCamera = new CCamera;
	m_pSkybox = new CSkybox;
	m_pShaderPrograms = new vector <CShaderProgram *>;
	m_pPlanarTerrain = new CPlane;
	m_scoreText = new CFreeTypeFont;
	m_lapText = new CFreeTypeFont;
	m_speedText = new CFreeTypeFont;
	m_gameOverText = new CFreeTypeFont;
	m_pSphere = new CSphere;


	m_track = new CCatmullRom;
	m_player = new CPlayer;

	for (int i = 0; i < numberOfCoins; i++)
	{
		CCoin* temp = new CCoin;
		m_coins.push_back(temp);
	}

	m_lamp = new CLamp;
	m_cone = new CCone;

	for (int i = 0; i < numberOfCones; i++)
	{
		CCone* temp = new CCone;
		m_cones.push_back(temp);
	}

	RECT dimensions = m_gameWindow.GetDimensions();

	int width = dimensions.right - dimensions.left;
	int height = dimensions.bottom - dimensions.top;

	// Set the orthographic and perspective projection matrices based on the image size
	m_pCamera->SetOrthographicProjectionMatrix(width, height); 
	m_pCamera->SetPerspectiveProjectionMatrix(45.0f, (float) width / (float) height, 0.5f, 5000.0f);

	// Load shaders
	vector<CShader> shShaders;
	vector<string> sShaderFileNames;
	sShaderFileNames.push_back("mainShader.vert");
	sShaderFileNames.push_back("mainShader.frag");
	sShaderFileNames.push_back("textShader.vert");
	sShaderFileNames.push_back("textShader.frag");
	sShaderFileNames.push_back("sphereShader.vert");
	sShaderFileNames.push_back("sphereShader.frag");
	sShaderFileNames.push_back("coinShader.vert");
	sShaderFileNames.push_back("coinShader.frag");
	sShaderFileNames.push_back("lampShader.vert");
	sShaderFileNames.push_back("lampShader.frag");

	for (int i = 0; i < (int) sShaderFileNames.size(); i++) {
		string sExt = sShaderFileNames[i].substr((int) sShaderFileNames[i].size()-4, 4);
		int iShaderType;
		if (sExt == "vert") iShaderType = GL_VERTEX_SHADER;
		else if (sExt == "frag") iShaderType = GL_FRAGMENT_SHADER;
		else if (sExt == "geom") iShaderType = GL_GEOMETRY_SHADER;
		else if (sExt == "tcnl") iShaderType = GL_TESS_CONTROL_SHADER;
		else iShaderType = GL_TESS_EVALUATION_SHADER;
		CShader shader;
		shader.LoadShader("resources\\shaders\\"+sShaderFileNames[i], iShaderType);
		shShaders.push_back(shader);
	}

	// Create the main shader program
	CShaderProgram *pMainProgram = new CShaderProgram;
	pMainProgram->CreateProgram();
	pMainProgram->AddShaderToProgram(&shShaders[0]);
	pMainProgram->AddShaderToProgram(&shShaders[1]);
	pMainProgram->LinkProgram();
	m_pShaderPrograms->push_back(pMainProgram);

	// Create a shader program for fonts
	CShaderProgram *pFontProgram = new CShaderProgram;
	pFontProgram->CreateProgram();
	pFontProgram->AddShaderToProgram(&shShaders[2]);
	pFontProgram->AddShaderToProgram(&shShaders[3]);
	pFontProgram->LinkProgram();
	m_pShaderPrograms->push_back(pFontProgram);

	// Create the sphere shader program
	CShaderProgram *pSphereProgram = new CShaderProgram;
	pSphereProgram->CreateProgram();
	pSphereProgram->AddShaderToProgram(&shShaders[4]);
	pSphereProgram->AddShaderToProgram(&shShaders[5]);
	pSphereProgram->LinkProgram();
	m_pShaderPrograms->push_back(pSphereProgram);

	CShaderProgram* pCoinProgram = new CShaderProgram;
	pCoinProgram->CreateProgram();
	pCoinProgram->AddShaderToProgram(&shShaders[6]);
	pCoinProgram->AddShaderToProgram(&shShaders[7]);
	pCoinProgram->LinkProgram();
	m_pShaderPrograms->push_back(pCoinProgram);

	CShaderProgram* pLampProgram = new CShaderProgram;
	pLampProgram->CreateProgram();
	pLampProgram->AddShaderToProgram(&shShaders[8]);
	pLampProgram->AddShaderToProgram(&shShaders[9]);
	pLampProgram->LinkProgram();
	m_pShaderPrograms->push_back(pLampProgram);

	// You can follow this pattern to load additional shaders

	// Create the skybox
	// Skybox downloaded from http://www.akimbo.in/forum/viewtopic.php?f=10&t=9
	m_pSkybox->Create(2500.0f);
	
	// Create the planar terrain
	m_pPlanarTerrain->Create("resources\\textures\\", "grassfloor01.jpg", 2000.0f, 2000.0f, 50.0f); // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013

	m_scoreText->LoadSystemFont("arial.ttf", 32);
	m_scoreText->SetShaderProgram(pFontProgram);
	m_lapText->LoadSystemFont("arial.ttf", 32);
	m_lapText->SetShaderProgram(pFontProgram);
	m_speedText->LoadSystemFont("arial.ttf", 32);
	m_speedText->SetShaderProgram(pFontProgram);
	m_gameOverText->LoadSystemFont("arial.ttf", 32);
	m_gameOverText->SetShaderProgram(pFontProgram);


	//Objects

	//Track
	m_track->CreateCentreline(500);
	m_track->SetHeight(5.f);
	m_track->CreateOffsetCurves();
	m_track->CreateRightRails();
	m_track->CreateLeftRails();
	m_track->CreateTrack();
	m_track->CreateRightOffRoad();
	m_track->CreateLeftOffRoad();

	//Player
	m_player->Init();

	//Coin
	for (int i = 0; i < m_coins.size(); i++)
	{
		m_coins[i]->Create(32, 4.f, 1.f);
		m_coins[i]->Init(m_track->RandomPoint() + glm::vec3(0.f, 4.f, 0.f));
	}

	m_lamp->Set(false);
	//2 -> height
	//4 -> Length
	m_lamp->SetSize(0.5f, 20.f, 0.5f, 15.f, 1.5f, 0.5f, 8.f);
	m_lamp->Create(32);
	m_lamp->SetPosition(m_track->LampPosition(0));
	SetLampOffsets();

	m_cone->Init(m_track->RandomPoint() + glm::vec3(0.f, 8.f, 0.f));

	for (int i = 0; i < m_cones.size(); i++)
	{
		m_cones[i]->Init(m_track->RandomPoint() + glm::vec3(0.f, 8.f, 0.f));
	}

	// Create a sphere
	m_pSphere->Create("resources\\textures\\", "dirtpile01.jpg", 25, 25);  // Texture downloaded from http://www.psionicgames.com/?page_id=26 on 24 Jan 2013
	glEnable(GL_CULL_FACE);


}

void Game::SetLampOffsets()
{
	m_positions[0] = 0;
	m_points[0] = m_lamp->Position();
	m_angles[0] = m_lamp->RotationAngle(m_track->CenterPoint(m_positions[0])) * (180 / M_PI);
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, m_points[0]);
	model = glm::rotate(model, m_angles[0], glm::vec3(0.f, 1.f, 0.f));

	m_matrices[0] = model;
	for (int i = 1; i < 20; i++)
	{
		m_positions[i] = m_positions[i - 1] + 20;
		m_points[i] = m_track->LampPosition(m_positions[i]);
		m_angles[i] = m_lamp->RotationAngle(m_track->CenterPoint(m_positions[i])) * (180 / M_PI);
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, m_points[i]);
		model = glm::rotate(model, m_angles[i], glm::vec3(0.f, 1.f, 0.f));

		m_matrices[i] = model;
	}
}

void Game::ResetCoins()
{
	for (int i = 0; i < m_coins.size(); i++)
	{
		m_coins[i]->Init(m_track->RandomPoint() + glm::vec3(0.f, 4.f, 0.f));
	}
}

void Game::ResetCones()
{
	for (int i = 0; i < m_cones.size(); i++)
	{
		m_cones[i]->Init(m_track->RandomPoint() + glm::vec3(0.f, 8.f, 0.f));
	}
}


// Render method runs repeatedly in a loop
void Game::Render() 
{
	
	// Clear the buffers and enable depth testing (z-buffering)
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Set up a matrix stack
	glutil::MatrixStack modelViewMatrixStack;
	modelViewMatrixStack.SetIdentity();

	// Use the main shader program 
	CShaderProgram *pMainProgram = (*m_pShaderPrograms)[0];
	pMainProgram->UseProgram();
	pMainProgram->SetUniform("bUseTexture", true);
	pMainProgram->SetUniform("sampler0", 0);
	// Note: cubemap and non-cubemap textures should not be mixed in the same texture unit.  Setting unit 10 to be a cubemap texture.
	int cubeMapTextureUnit = 10; 
	pMainProgram->SetUniform("CubeMapTex", cubeMapTextureUnit);


	// Set the projection matrix
	pMainProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	// Call LookAt to create the view matrix and put this on the modelViewMatrix stack. 
	// Store the view matrix and the normal matrix associated with the view matrix for later (they're useful for lighting -- since lighting is done in eye coordinates)
	modelViewMatrixStack.LookAt(m_pCamera->GetPosition(), m_pCamera->GetView(), m_pCamera->GetUpVector());
	glm::mat4 viewMatrix = modelViewMatrixStack.Top();
	glm::mat3 viewNormalMatrix = m_pCamera->ComputeNormalMatrix(viewMatrix);

	
	//glm::vec3 positions[3] = { m_lamp->Position(), m_track->LampPosition(40), m_track->LampPosition(20) };

	// Set light and materials in main shader program
	glm::vec4 lightPosition1(0, 100, 0, 1); // Position of light source *in world coordinates*
	pMainProgram->SetUniform("light1.position", viewMatrix*lightPosition1); // Position of light source *in eye coordinates*
	pMainProgram->SetUniform("light1.La", glm::vec3(1.f));		// Ambient colour of light
	pMainProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pMainProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pMainProgram->SetUniform("material1.Ma", glm::vec3(0.5f));	// Ambient material reflectance
	pMainProgram->SetUniform("material1.Md", glm::vec3(0.5f));	// Diffuse material reflectance
	pMainProgram->SetUniform("material1.Ms", glm::vec3(0.5f));	// Specular material reflectance
	pMainProgram->SetUniform("material1.shininess", 15.0f);		// Shininess material property

	// Render the skybox and terrain with full ambient reflectance 
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("renderSkybox", true);
	// Translate the modelview matrix to the camera eye point so skybox stays centred around camera
	glm::vec3 vEye = m_pCamera->GetPosition();
	modelViewMatrixStack.Translate(vEye);
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSkybox->Render(cubeMapTextureUnit);
	pMainProgram->SetUniform("renderSkybox", false);
	modelViewMatrixStack.Pop();

	// Render the planar terrain
	modelViewMatrixStack.Push();
	pMainProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pMainProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pPlanarTerrain->Render();
	modelViewMatrixStack.Pop();


	// Switch to the sphere program
	CShaderProgram *pSphereProgram = (*m_pShaderPrograms)[2];
	pSphereProgram->UseProgram();


	pSphereProgram->SetUniform("bUseTexture", true);
	pSphereProgram->SetUniform("sampler0", 0);
	// Set light and materials in sphere programme
	pSphereProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Light position in eye coordinates
	pSphereProgram->SetUniform("light1.La", glm::vec3(0.2f));
	pSphereProgram->SetUniform("light1.Ld", glm::vec3(0.2f));
	pSphereProgram->SetUniform("light1.Ls", glm::vec3(0.2f));
	//pSphereProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	//pSphereProgram->SetUniform("light1.exponent", 20.f);
	//pSphereProgram->SetUniform("light1.cutoff", 30.f);
	pSphereProgram->SetUniform("material1.shininess", 15.0f);
	pSphereProgram->SetUniform("material1.Ma", glm::vec3(0.2f, 0.2f, 0.2f));
	pSphereProgram->SetUniform("material1.Md", glm::vec3(0.2f, 0.2f, 0.2f));
	pSphereProgram->SetUniform("material1.Ms", glm::vec3(0.2f, 0.2f, 0.2f));

	/*pSphereProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Light position in eye coordinates
	pSphereProgram->SetUniform("light1.La", glm::vec3(0.2f));
	pSphereProgram->SetUniform("light1.Ld", glm::vec3(1.f));
	pSphereProgram->SetUniform("light1.Ls", glm::vec3(1.f));
	//pSphereProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	//pSphereProgram->SetUniform("light1.exponent", 20.f);
	//pSphereProgram->SetUniform("light1.cutoff", 30.f);
	pSphereProgram->SetUniform("material1.shininess", 15.0f);
	pSphereProgram->SetUniform("material1.Ma", glm::vec3(1.f));
	pSphereProgram->SetUniform("material1.Md", glm::vec3(1.f));
	pSphereProgram->SetUniform("material1.Ms", glm::vec3(1.f));*/

	//Check closest point, if it is 0, reset lamp to 0, otherwise keep movit it up once you passed it.

	glm::vec4 lightPosition2{m_points[currentPos].x, 200.f, m_points[currentPos].z, 1.f };
	pSphereProgram->SetUniform("light2.position", viewMatrix * lightPosition2);
	pSphereProgram->SetUniform("light2.La", glm::vec3(2.0f, 2.0f, 2.0f));
	pSphereProgram->SetUniform("light2.Ld", glm::vec3(5.0f, 5.0f, 5.0f));
	pSphereProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	pSphereProgram->SetUniform("light2.exponent", 30.f);
	pSphereProgram->SetUniform("light2.cutoff", 50.f);

	/*glm::vec4 lightPosition3{ m_track->LampPosition(40).x, 200.f, m_track->LampPosition(40).y, 1.f };
	pSphereProgram->SetUniform("light2.position", viewMatrix* lightPosition3);
	pSphereProgram->SetUniform("light2.La", glm::vec3(2.0f, 2.0f, 2.0f));
	pSphereProgram->SetUniform("light2.Ld", glm::vec3(5.0f, 5.0f, 5.0f));
	pSphereProgram->SetUniform("light2.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix* glm::vec3(0.f, -1.f, 0.f)));
	pSphereProgram->SetUniform("light2.exponent", 30.f);
	pSphereProgram->SetUniform("light2.cutoff", 50.f);*/

	//Move the light from lamp to lamp


	/*glm::vec4 lightPosition3{ m_track->LampPosition(40), 1.f };
	pSphereProgram->SetUniform("light3.position", viewMatrix* lightPosition2);
	pSphereProgram->SetUniform("light3.La", glm::vec3(2.0f, 2.0f, 2.0f));
	pSphereProgram->SetUniform("light3.Ld", glm::vec3(5.0f, 5.0f, 5.0f));
	pSphereProgram->SetUniform("light3.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light3.direction", glm::normalize(viewNormalMatrix* glm::vec3(0.f, -1.f, 0.f)));
	pSphereProgram->SetUniform("light3.exponent", 30.f);
	pSphereProgram->SetUniform("light3.cutoff", 50.f);*/



	pSphereProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());


	// Render the sphere
	/*modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(glm::vec3(0.0f, 5.0f, 0.0f));
	modelViewMatrixStack.Scale(5.0f);
	pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	m_pSphere->Render();
	for (int x = 0; x < 10; x++)
	{
		for (int z = 0; z < 10; z++)
		{
			modelViewMatrixStack.Push();
			modelViewMatrixStack.Translate(glm::vec3(2.5f * x, 0.f, -2.5f * z));
			pSphereProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());
			pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
			pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
			m_pSphere->Render();
			modelViewMatrixStack.Pop();
		}
	}
	modelViewMatrixStack.Pop();*/

	modelViewMatrixStack.Push();
	pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	//DEBUGGING
	//m_track->RenderCentreline();
	//m_track->RenderOffsetCurves();
	//pMainProgram->SetUniform("bUseTexture", true);
	m_track->RenderRightRails();
	m_track->RenderLeftRails();
	m_track->RenderTrack();
	m_track->RenderRightOffRoad();
	m_track->RenderLeftOffRoad();
	modelViewMatrixStack.Pop();

	/*glm::vec4 lightPosition2{ m_lamp->Position().x,200.f, m_lamp->Position().z, 1.f };
	pSphereProgram->SetUniform("light3.position", viewMatrix* lightPosition2);
	pSphereProgram->SetUniform("light3.La", glm::vec3(2.0f, 2.0f, 2.0f));
	pSphereProgram->SetUniform("light3.Ld", glm::vec3(5.0f, 5.0f, 5.0f));
	pSphereProgram->SetUniform("light.Ls", glm::vec3(1.0f, 1.0f, 1.0f));
	pSphereProgram->SetUniform("light2.direction", glm::normalize(viewNormalMatrix* glm::vec3(0.f, -1.f, 0.f)));
	pSphereProgram->SetUniform("light2.exponent", 30.f);
	pSphereProgram->SetUniform("light2.cutoff", 50.f);*/


	modelViewMatrixStack.Push();
	modelViewMatrixStack.Translate(m_player->Position());
	modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.f, 0.f), m_player->Rotation());
	modelViewMatrixStack.Scale(2.5f);
	modelViewMatrixStack *= m_carOrientation;
	pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pSphereProgram->SetUniform("bUseTexture", true);
	m_player->Render();
	modelViewMatrixStack.Pop();

	if (!m_gameOver)
	{
		for (int i = 0; i < m_cones.size(); i++)
		{
			if (m_cones[i]->isActive())
			{
				modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(m_cones[i]->Position());
				modelViewMatrixStack.Scale(2.f);
				pSphereProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pSphereProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				pSphereProgram->SetUniform("bUseTexture", true);
				m_cones[i]->Render();
				modelViewMatrixStack.Pop();
			}
		}
	}



	CShaderProgram* pCoinProgram = (*m_pShaderPrograms)[3];
	pCoinProgram->UseProgram();
	pCoinProgram->SetUniform("bUseTexture", true);
	pCoinProgram->SetUniform("sampler0", 0);
	pCoinProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());


	pCoinProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Position of light source *in eye coordinates*
	pCoinProgram->SetUniform("light1.La", glm::vec3(1.f));		// Ambient colour of light
	pCoinProgram->SetUniform("light1.Ld", glm::vec3(1.0f));		// Diffuse colour of light
	pCoinProgram->SetUniform("light1.Ls", glm::vec3(1.0f));		// Specular colour of light
	pCoinProgram->SetUniform("material1.Ma", glm::vec3(1.0f));	// Ambient material reflectance
	pCoinProgram->SetUniform("material1.Md", glm::vec3(1.0f));	// Diffuse material reflectance
	pCoinProgram->SetUniform("material1.Ms", glm::vec3(2.0f));	// Specular material reflectance
	pCoinProgram->SetUniform("material1.shininess", 50.0f);		// Shininess material property

	if (!m_gameOver)
	{

		pCoinProgram->SetUniform("t", m_t);
		for (int i = 0; i < m_coins.size(); i++)
		{
			if (m_coins[i]->isActive())
			{
				modelViewMatrixStack.Push();
				modelViewMatrixStack.Translate(m_coins[i]->Position());
				pCoinProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
				pCoinProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
				pCoinProgram->SetUniform("bUseTexture", true);
				m_coins[i]->Render();
				modelViewMatrixStack.Pop();
			}
		}
	}
	// Draw the 2D graphics after the 3D graphics
	CShaderProgram* pLampProgram = (*m_pShaderPrograms)[4];
	pLampProgram->UseProgram();
	pLampProgram->SetUniform("bUseTexture", true);
	pLampProgram->SetUniform("sampler0", 0);
	pLampProgram->SetUniform("matrices.projMatrix", m_pCamera->GetPerspectiveProjectionMatrix());

	pLampProgram->SetUniform("light1.position", viewMatrix * lightPosition1); // Light position in eye coordinates
	pLampProgram->SetUniform("light1.La", glm::vec3(0.5f));
	pLampProgram->SetUniform("light1.Ld", glm::vec3(0.5f));
	pLampProgram->SetUniform("light1.Ls", glm::vec3(0.5f));
	//pSphereProgram->SetUniform("light1.direction", glm::normalize(viewNormalMatrix * glm::vec3(0.f, -1.f, 0.f)));
	//pSphereProgram->SetUniform("light1.exponent", 20.f);
	//pSphereProgram->SetUniform("light1.cutoff", 30.f);
	pLampProgram->SetUniform("material1.shininess", 15.0f);
	pLampProgram->SetUniform("material1.Ma", glm::vec3(1.f));
	pLampProgram->SetUniform("material1.Md", glm::vec3(1.f));
	pLampProgram->SetUniform("material1.Ms", glm::vec3(1.f));


	//pLampProgram->SetUniform("positions[1]", m_lamp->Position()+glm::vec3(5.f, 5.f, 5.f));

	/*float angles[3] = { m_lamp->RotationAngle(m_track->CenterPoint(0)) * (180 / M_PI),
	m_lamp->RotationAngle(m_track->CenterPoint(40)) * (180 / M_PI),
	m_lamp->RotationAngle(m_track->CenterPoint(20)) * (180 / M_PI) };*/
	/*glm::mat4 matrices[3];
	for (int i = 0; i < 3; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, positions[i]);
		model = glm::rotate(model,  angles[i], glm::vec3(0.f, 1.f, 0.f));

		matrices[i] = model;
	}*/
/*	GLint myLoc = glGetUniformLocation(pLampProgram->GetProgramID(), "positions");
	glProgramUniform3fv(pLampProgram->GetProgramID(), myLoc, 2, (GLfloat*)positions);*/
	pLampProgram->SetUniform("positions", m_points, 20);
	pLampProgram->SetUniform("angles", m_angles, 20);
	pLampProgram->SetUniform("translationMatrices", m_matrices, 20);

	/*float angle = m_lamp->RotationAngle(m_track->CenterPoint(30));
	angle *= 180 / M_PI;*/
	modelViewMatrixStack.Push();
	//modelViewMatrixStack.Translate(m_lamp->Position());
	//modelViewMatrixStack.Rotate(glm::vec3(0.f, 1.f, .0f), angle);
	pLampProgram->SetUniform("matrices.modelViewMatrix", modelViewMatrixStack.Top());
	pLampProgram->SetUniform("matrices.normalMatrix", m_pCamera->ComputeNormalMatrix(modelViewMatrixStack.Top()));
	pLampProgram->SetUniform("bUseTexture", true);
	m_lamp->Render();
	modelViewMatrixStack.Pop();


	DisplayFrameRate();

	// Swap buffers to show the rendered image
	SwapBuffers(m_gameWindow.Hdc());

}

// Update method runs repeatedly with the Render method
void Game::Update() 
{
	

	if (m_track->ClosestPoint(m_player->Position()) >= m_positions[currentPos]+ 5 && m_track->ClosestPoint(m_player->Position()) < m_positions[currentPos] + 10)
	{
		if (currentPos >= 19)
		{
			currentPos = 0;
		}
		else
		{
			currentPos++;
		}
	}

	// Update the camera using the amount of time that has elapsed to avoid framerate dependent motion
	m_pCamera->Update(m_dt*2.5f);

	m_t += (float)(0.0025f * m_dt);


	if (!m_debug)
	{
		//Moving the camera along the track
		m_currentDistance += m_dt * m_player->Speed();

		m_track->Sample(m_currentDistance, p);


		m_track->Sample(m_currentDistance + 1.f, pNext);

		//TNB frame
		T = glm::normalize(pNext - p);
		up = glm::rotate(glm::vec3(0.f, 1.f, 0.f), m_jitterAngle, T);
		N = glm::normalize(glm::cross(T, up));
		B = glm::normalize(glm::cross(N, T));
		//Set the camera and player positions here
		m_cameraPosition = p + 2.f * B + m_cameraOffset * N;
		m_player->Update(m_cameraPosition);

		m_carOrientation = glm::mat4(glm::mat3(T, B, N));

		if (!m_gameOver)
		{

		
			//Camera follow:
			if (!m_fpsView)
			{
				m_pCamera->Set(m_cameraPosition + 25.f * B - 35.f * T, p + 40.f * T + m_cameraOffset * N, up);
			}
			else
			{
				m_pCamera->Set(m_cameraPosition + 1.f * T + 3.f * B + 0.15f * N, p + 40.f * T + m_cameraOffset * N, up);
			}

			if (m_player->Speed() >= 0.f)
			{
				m_player->SetCurrentPoint(m_track->ClosestPoint(p));
			}
			if (m_track->isOffRoading(m_cameraPosition, m_player->CurrentPoint()))
			{
				CameraJitter();
			}
			else
			{
				m_jitterAngle = 0.f;
			}

			if (m_track->hasCollided(m_cameraPosition, m_player->CurrentPoint()))
			{
				ResetPlayer();
			}
		}
		else
		{
			m_pCamera->Set((m_cameraPosition + 10.f*B) * 1.25f, p  + m_cameraOffset * N, up);
		}
	}

	//Player-coin collisions
	if (!m_gameOver)
	{

	
		for (int i = 0; i < m_coins.size(); i++)
		{

			if (m_coins[i]->isActive())
			{
				if (glm::distance(m_coins[i]->Position(), m_player->Position()) <= 20.f)
				{
					if (m_coins[i]->Collided(m_player->Position()))
					{
						m_player->IncrementScore(50.f);
						m_coins[i]->SetActive(false);
					}
				}
			}
		}


		//Player-cone collision
		for (int i = 0; i < m_cones.size(); i++)
		{

			if (m_cones[i]->isActive())
			{
				if (m_cones[i]->Collided(m_player->Position()))
				{
					m_player->IncrementScore(-50.f);
					m_player->IncrementSpeed(-(m_player->Speed() / 2));
					m_cones[i]->SetActive(false);
				}
			}
		}


		//Lap counter + gameOver;

		if (m_track->CurrentLap(m_currentDistance, m_lapCount) >= 1)
		{
			ResetCoins();
			ResetCones();
			m_lapCount += 1;
		}
		//m_lapCount += m_track->CurrentLap(m_currentDistance, m_lapCount);
		if (m_lapCount >= 3)
		{
			m_gameOver = true;
			m_cameraOffset = 0;
			m_player->SetSpeed(0.25f);
		}
	}

}

void Game::CameraJitter()
{
	//sin/2+0.5f
	if (!m_fpsView && m_player->Speed() >= 0.005f)
	{
		m_player->IncrementSpeed(-0.0001f);
		m_jitterValue = sin(m_dt) / 2.f + 0.5f;
		m_jitterAngle = m_jitterValue * 1.5f ;
	}
	else if (m_player->Speed() >= 0.005f)
	{
		m_player->IncrementSpeed(-0.0001f);
		m_jitterValue = sin(m_dt) / 2.f + 0.5f;
		m_jitterAngle = m_jitterValue * 1.8f;

	}
}

void Game::ResetPlayer()
{
	m_player->IncrementSpeed(-m_player->Speed());
	m_currentDistance = m_player->LastPosition();
	m_player->SetRotation(0.f);
	m_cameraOffset = 0.f;
}



void Game::DisplayFrameRate()
{


	CShaderProgram *fontProgram = (*m_pShaderPrograms)[1];

	RECT dimensions = m_gameWindow.GetDimensions();
	int height = dimensions.bottom - dimensions.top;
	int width = dimensions.right - dimensions.left;

	// Increase the elapsed time and frame counter
	/*m_elapsedTime += m_dt;
	m_frameCount++;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
	if (m_elapsedTime > 1000)
    {
		m_elapsedTime = 0;
		m_framesPerSecond = m_frameCount;

		// Reset the frames per second
		m_frameCount = 0;
    }*/


		// Use the font shader program and render the text
		fontProgram->UseProgram();
		glDisable(GL_DEPTH_TEST);
		fontProgram->SetUniform("matrices.modelViewMatrix", glm::mat4(1));
		fontProgram->SetUniform("matrices.projMatrix", m_pCamera->GetOrthographicProjectionMatrix());
		fontProgram->SetUniform("vColour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		//m_scoreText->Render(20, height - 20, 20, "Offrodaing: %d\n Point: %i\nSpeed: %.3f\n Jitter: %.3f", m_track->isOffRoading(m_cameraPosition, m_player->CurrentPoint()), m_player->CurrentPoint(), m_player->Speed(), m_jitterAngle);
		if (!m_gameOver)
		{
			m_scoreText->Render(20, height-20, 20, "Score: %i", m_player->Score());
			m_lapText->Render(width/2-40, height - 20, 20, "Laps: %i/3", m_lapCount);
			m_speedText->Render(20, 20, 20, "%.0f mph",m_player->Speed()*200.f);
		}
		else
		{
			m_scoreText->Render(width/2-40, height/2 + 50, 32, "Score: %i", m_player->Score());
			m_gameOverText->Render(width / 2 - 160, height / 2 - 50, 64, "GAME OVER");
		}
		
		//m_lapText in top middle
		//m_score in top left
	
}

// The game loop runs repeatedly until game over
void Game::GameLoop()
{
	/*
	// Fixed timer
	dDt = pHighResolutionTimer->Elapsed();
	if (dDt > 1000.0 / (double) Game::FPS) {
		pHighResolutionTimer->Start();
		Update();
		Render();
	}
	*/
	
	
	// Variable timer
	m_pHighResolutionTimer->Start();
	Update();
	Render();
	m_dt = m_pHighResolutionTimer->Elapsed();
	

}


WPARAM Game::Execute() 
{
	m_pHighResolutionTimer = new CHighResolutionTimer;
	m_gameWindow.Init(m_hInstance);

	if(!m_gameWindow.Hdc()) {
		return 1;
	}

	Initialise();

	m_pHighResolutionTimer->Start();

	
	MSG msg;

	while(1) {													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
			if(msg.message == WM_QUIT) {
				break;
			}

			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		} else if (m_appActive) {
			GameLoop();
		} 
		else Sleep(200); // Do not consume processor power if application isn't active
	}

	m_gameWindow.Deinit();

	return(msg.wParam);
}

LRESULT Game::ProcessEvents(HWND window,UINT message, WPARAM w_param, LPARAM l_param) 
{
	LRESULT result = 0;

	switch (message) {


	case WM_ACTIVATE:
	{
		switch(LOWORD(w_param))
		{
			case WA_ACTIVE:
			case WA_CLICKACTIVE:
				m_appActive = true;
				m_pHighResolutionTimer->Start();
				break;
			case WA_INACTIVE:
				m_appActive = false;
				break;
		}
		break;
		}

	case WM_SIZE:
			RECT dimensions;
			GetClientRect(window, &dimensions);
			m_gameWindow.SetDimensions(dimensions);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(window, &ps);
		EndPaint(window, &ps);
		break;
	case WM_KEYDOWN:
		switch (w_param) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		//Car movement
		case 'W':
			if (!m_debug && !m_gameOver)
			{
				m_player->IncrementSpeed(0.002f);
			}
			break;
		case 'S':
			if (!m_debug && !m_gameOver)
			{

				m_player->IncrementSpeed(-0.002f);
			}
			break;
		case 'D':
			if (!m_debug && !m_gameOver)
			{
				m_cameraOffset += 0.5f * (m_player->Speed() * 10.f);
				m_player->SetRotation(-25.f);
				if (!m_track->isOffRoading(m_cameraPosition, m_player->CurrentPoint()))
				{
					m_player->SetLastPosition(m_currentDistance);
				}
			}
			break;
		case 'A':
			if (!m_debug && !m_gameOver)
			{
				m_cameraOffset -= 0.5f*(m_player->Speed()*10.f);
				m_player->SetRotation(25.f);
				if (!m_track->isOffRoading(m_cameraPosition, m_player->CurrentPoint()))
				{
					m_player->SetLastPosition(m_currentDistance);
				}
			}
			break;
		case 'V':
			if (!m_gameOver && !m_debug)
			{
				m_fpsView = !m_fpsView;
			}

			break;
		case 'G':
			m_debug = !m_debug;
			break;
		}
		break;
	case WM_KEYUP:
		switch (w_param) {
		case 'D':
			m_player->SetRotation(0.f);
			break;
		case 'A':
			m_player->SetRotation(0.f);
			break;
		default:
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		result = DefWindowProc(window, message, w_param, l_param);
		break;
	}

	return result;
}

Game& Game::GetInstance() 
{
	static Game instance;

	return instance;
}

void Game::SetHinstance(HINSTANCE hinstance) 
{
	m_hInstance = hinstance;
}

LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	return Game::GetInstance().ProcessEvents(window, message, w_param, l_param);
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE, PSTR, int) 
{
	Game &game = Game::GetInstance();
	game.SetHinstance(hinstance);

	return game.Execute();
}
