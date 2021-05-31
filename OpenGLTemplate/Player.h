#pragma once

#include "Common.h"


class COpenAssetImportMesh;

class CPlayer
{
public:
	CPlayer();
	~CPlayer();
	void Init();
	void Update(glm::vec3 newPosition);
	void Render();
	void Reset();
	glm::vec3 Position();
	int Score();
	float Speed();
	int CurrentPoint();
	int LastPosition();
	float Rotation();
	void IncrementScore(int amount);
	void IncrementSpeed(float amount);
	void SetCurrentPoint(int newPoint);
	void SetLastPosition(float newPoint);
	void SetRotation(float newRotation);
	void SetSpeed(float newSpeed);
private:
	COpenAssetImportMesh* m_carMesh;
	glm::vec3 m_position;
	int m_score = -50;
	float m_speed = 0.f;
	float m_maxSpeed = 0.5f;
	//The closest point to the car on the road
	int m_currentPoint;
	//Stores the distance the player is along the road.
	float m_lastPosition;
	float m_rotation = 0.f;
};