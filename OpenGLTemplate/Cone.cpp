#include "Cone.h"
#include "OpenAssetImportMesh.h"


CCone::CCone() 
{
	m_coneMesh = new COpenAssetImportMesh;
	m_coneMesh->Load("resources\\models\\Cones\\trafficCone.obj");
}

CCone::~CCone() {}

void CCone::Init(const glm::vec3 startPosition)
{
	m_position = startPosition;
	m_active = true;
}

void CCone::Render()
{
	m_coneMesh->Render();
}

bool CCone::Collided(glm::vec3 otherPostion)
{
	if (glm::distance(otherPostion, m_position) <= 10.f)
	{
		return true;
	}
	return false;
}


//Getters and setters
glm::vec3 CCone::Position()
{
	return m_position;
}

void CCone::SetPosition(const glm::vec3 newPosition)
{
	m_position = newPosition;
}

bool CCone::isActive()
{
	return m_active;
}

void CCone::SetActive(bool active)
{
	m_active = active;
}