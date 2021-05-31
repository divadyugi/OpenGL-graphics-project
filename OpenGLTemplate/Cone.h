#pragma once

#include "Common.h"

class COpenAssetImportMesh;

class CCone
{
public:
	CCone();
	~CCone();
	void Init(const glm::vec3 startPosition);
	void Render();
	void Release();

	bool Collided(glm::vec3 otherPosition);
	void SetPosition(const glm::vec3 newPosition);
	glm::vec3 Position();
	bool isActive();
	void SetActive(bool active);
private:
	COpenAssetImportMesh* m_coneMesh;
	glm::vec3 m_position;

	bool m_active;
};