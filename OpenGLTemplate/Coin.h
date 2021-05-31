#pragma once

#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class CCoin
{
public:
	CCoin();
	~CCoin();
	void Init(const glm::vec3 startPosition);
	void Create(int stacks, int radius, float width);
	void Render();
	void Release();
	//If collided, deactivate coin and award 100 score to player.
	bool Collided(glm::vec3 otherPosition);
	void SetPosition(const glm::vec3 newPosition);
	glm::vec3 Position();
	bool isActive();
	void SetActive(bool active);
private:

	//Face texture
	CTexture m_faceTexture;
	//Connection texture
	CTexture m_connectionTexture;

	//Front of the coin
	std::vector<glm::vec3> m_frontVertices;
	GLuint m_frontVao;
	CVertexBufferObject m_frontVbo;

	//Back of the coin;
	std::vector<glm::vec3> m_backVertices;
	GLuint m_backVao;
	CVertexBufferObject m_backVbo;
	std::vector<glm::vec2> m_backTexture;

	//Connection
	std::vector<glm::vec3> m_connectorVertices;
	std::vector<glm::vec2> m_connectorTextures;
	std::vector<glm::vec3> m_connectorNormals;
	GLuint m_connectorVao;
	CVertexBufferObject m_connectorVbo;


	glm::vec3 m_position;

	bool m_active;
	//Debug lines:
	/*std::vector<glm::vec3> d_linePoints;
	GLuint d_lineVao;
	CVertexBufferObject d_lineVbo;*/
};