#include "Player.h"
#include "OpenAssetImportMesh.h"

CPlayer::CPlayer() {}

CPlayer::~CPlayer() {}

void CPlayer::Init()
{
	m_carMesh = new COpenAssetImportMesh;
	m_carMesh->Load("resources\\models\\Car\\350z_triangulated.obj");
}

void CPlayer::Update(glm::vec3 newPosition)
{
	m_position = newPosition;
}

void CPlayer::Render()
{
	m_carMesh->Render();
}

//Getters and setters

glm::vec3 CPlayer::Position()
{
	return m_position;
}

int CPlayer::Score()
{
	return m_score;
}

float CPlayer::Speed()
{
	return m_speed;
}

int CPlayer::CurrentPoint()
{
	return m_currentPoint;
}

int CPlayer::LastPosition()
{
	return m_lastPosition;
}

float CPlayer::Rotation()
{
	return m_rotation;
}


void CPlayer::IncrementScore(int amount)
{
	m_score += amount;
}

void CPlayer::IncrementSpeed(float amount)
{
	m_speed += amount;
	if (m_speed >= m_maxSpeed)
	{
		m_speed = m_maxSpeed;
	}
	else if (m_speed <= 0.f)
	{
		m_speed = 0;
	}
}

void CPlayer::SetCurrentPoint(int newPoint)
{
	m_currentPoint = newPoint;
}

void CPlayer::SetLastPosition(float newPoint)
{
	m_lastPosition = newPoint;
}

void CPlayer::SetRotation(float newRotation)
{
	m_rotation = newRotation;
}

void CPlayer::SetSpeed(float newSpeed)
{
	m_speed = newSpeed;
}