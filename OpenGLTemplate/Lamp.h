#pragma once

#include "Common.h"
#include "VertexBufferObject.h"
#include "Texture.h"

class CLamp
{
public:
	CLamp();
	~CLamp();
	void Create(int stacks);
	void SetSize(float postRadius, float postHeight, float overhangRadius, float overhangLength, float coneBaseRadius, float coneLength, float coneEndRadius);
	void Set(bool shining);
	void Render();
	void Release();
	glm::vec3 Position();
	void SetPosition(glm::vec3 newPosition);
	float RotationAngle(glm::vec3 targetPoint);
	int Count();
	void SetCount(int count);

private:

	int m_count;

	glm::vec3 m_position;

	//Is the light turned on?
	bool m_shining = false;

	CTexture m_texture;

	//Lamp post
	//Base
	float m_postRadius;
	float m_postHeight;
	std::vector<glm::vec3> m_postBaseVertices;
	GLuint m_postVao;
	CVertexBufferObject m_postVbo;

	//Post Connector
	std::vector<glm::vec3> m_postConnectorVertices;
	std::vector<glm::vec2> m_postConnectorTextures;

	//Post Top
	std::vector<glm::vec3> m_postTopVertices;
	std::vector<glm::vec2> m_postTopTextures;

	//Overhang
	float m_overhangRadius;
	float m_overhangLength;
	std::vector<glm::vec3> m_overhangBaseVertices;
	GLuint m_overHangVao;
	CVertexBufferObject m_overHangVbo;

	//Overhang Connector
	std::vector<glm::vec3> m_overhangConnectorVertices;
	std::vector<glm::vec2> m_overhangConnectorTextures;

	//Overhang top
	std::vector<glm::vec3> m_overhangTopVertices;
	std::vector<glm::vec2> m_overhangTopTextures;

	//Lightbulb cone
	float m_coneBaseRadius;
	float m_coneLength;
	float m_coneEndRadius;
	std::vector<glm::vec3> m_coneVertices;
	GLuint m_coneVao;
	CVertexBufferObject m_coneVbo;

	//Cone connector
	std::vector<glm::vec3> m_coneConnector;
	std::vector<glm::vec2> m_coneConnectorTextures;

	//Cone Top
	std::vector<glm::vec3> m_coneTopVertices;
	std::vector<glm::vec2> m_coneTopTextures;
};