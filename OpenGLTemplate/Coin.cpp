#include "Coin.h"

#define _USE_MATH_DEFINES

#include <math.h>

CCoin::CCoin() {}
//TODO: add release function here
CCoin::~CCoin()
{
	Release();
}

void CCoin::Init(const glm::vec3 startPosition)
{
	m_position = startPosition;
	m_active = true;
}


void CCoin::Create(int stacks, int radius, float width)
{

	//Texture initialisation
	//TODO: replace constant filename with creation argument, filename
	m_faceTexture.Load("resources\\textures\\coin.png");
	m_faceTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_faceTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_faceTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_faceTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

	m_connectionTexture.Load("resources\\textures\\coin_connector.png");
	m_connectionTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_connectionTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_connectionTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_connectionTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);



	glGenVertexArrays(1, &m_frontVao);
	glBindVertexArray(m_frontVao);

	m_frontVbo.Create();
	m_frontVbo.Bind();


	//Center of the coin, could move it to be a private variable
	glm::vec3 coin_origin{ 0.f,0.f,0.f };
	glm::vec2 texOrigin(0.5f, 0.5f);
	glm::vec2 texCoords(0.f, 0.f);
	glm::vec3 normalCoords(0.f, 0.f, 1.f);

	m_frontVbo.AddData(&coin_origin, sizeof(glm::vec3));
	m_frontVbo.AddData(&texOrigin, sizeof(glm::vec2));
	m_frontVbo.AddData(&normalCoords, sizeof(glm::vec3));

	/*d_textures.push_back(glm::vec2(1.f, 0.5f));
	d_textures.push_back(glm::vec2(0.75f, 0.75f));
	d_textures.push_back(glm::vec2(0.5f, 1.f));
	d_textures.push_back(glm::vec2(0.25f, 0.75f));
	d_textures.push_back(glm::vec2(0.f, 0.5f));
	d_textures.push_back(glm::vec2(0.25f, 0.25f));
	d_textures.push_back(glm::vec2(0.5f, 0.f));
	d_textures.push_back(glm::vec2(0.75f, 0.25f));
	d_textures.push_back(glm::vec2(1.f, 0.5f));*/

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(cos(theta) * (float)radius, sin(theta) * (float)radius, 0.f);
		//Texture x = cos(Theta)
		//Texture y = sin(theta)?
		//Maybe need to calculate using difference between origin, and normalize? or just divide by radius.
		//glm::vec2 tex{ cos(theta),sin(theta) };

		//Add the origin, and clamp it between 0 and 1 for each value
		//only do this for certain values
		//tex = glm::clamp(tex + texOrigin, glm::vec2(0.f,0.f),glm::vec2(1.f,1.f));
		//tex = glm::clamp(tex, glm::vec2(0.f, 0.f), glm::vec2(1.f, 1.f));
		//d_textures.push_back(tex);
		//Texture calculation from: https://community.khronos.org/t/texture-mapping-onto-a-disk/69660
		glm::vec3 normal = glm::normalize(glm::cross(next, coin_origin));

		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };


		m_frontVertices.push_back(next);
		m_frontVbo.AddData(&next, sizeof(glm::vec3));
		m_frontVbo.AddData(&tex, sizeof(glm::vec2));
		m_frontVbo.AddData(&normal, sizeof(glm::vec3));
	}

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);

	m_frontVbo.UploadDataToGPU(GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	//Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//I could probably render these two through one VAO, since the middle won't be seen anyway...but should I?

	//Create the back face
	glGenVertexArrays(1, &m_backVao);
	glBindVertexArray(m_backVao);

	m_backVbo.Create();
	m_backVbo.Bind();


	glm::vec3 backCoin_origin{ 0.f,0.f,-width };

	m_backVbo.AddData(&backCoin_origin, sizeof(glm::vec3));
	m_backVbo.AddData(&texOrigin, sizeof(glm::vec2));
	m_backVbo.AddData(&normalCoords, sizeof(glm::vec3));

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(cos(theta) * (float)radius, sin(theta) * (float)radius, -width);

		m_backVertices.push_back(next);
		m_backTexture.push_back(glm::vec2(((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2)));
		/*m_backVbo.AddData(&next, sizeof(glm::vec3));
		m_backVbo.AddData(&texCoords, sizeof(glm::vec2));
		m_backVbo.AddData(&normalCoords, sizeof(glm::vec3));*/
	}

	for (int i = m_backVertices.size() - 1; i >= 0; i--)
	{
		m_backVbo.AddData(&m_backVertices[i], sizeof(glm::vec3));
		m_backVbo.AddData(&m_backTexture[m_backTexture.size() - i - 1], sizeof(glm::vec2));
		glm::vec3 normal = glm::normalize(glm::cross(backCoin_origin, m_backVertices[i]));
		m_backVbo.AddData(&normal, sizeof(glm::vec3));
	}


	m_backVbo.UploadDataToGPU(GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	//Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	for (int i = 0; i < m_frontVertices.size(); i++)
	{
		glm::vec3 frontFirst{};
		glm::vec3 frontSecond{};
		glm::vec3 backFirst{};
		glm::vec3 backSecond{};
		if (i == m_frontVertices.size() - 1)
		{
			frontFirst = m_frontVertices.at(m_frontVertices.size() - 1);
			frontSecond = m_frontVertices.at(0);

			backFirst = m_backVertices.at(m_backVertices.size() - 1);
			backSecond = m_backVertices.at(0);
		}
		else
		{
			frontFirst = m_frontVertices.at(i);
			frontSecond = m_frontVertices.at(i + 1);

			backFirst = m_backVertices.at(i);
			backSecond = m_backVertices.at(i + 1);
		}
		glm::vec3 normal1 = glm::cross(frontFirst, backFirst);
		m_connectorVertices.push_back(frontFirst);
		m_connectorTextures.push_back(glm::vec2(0.f, 1.f));
		m_connectorNormals.push_back(normal1);
		m_connectorVertices.push_back(frontSecond);
		m_connectorTextures.push_back(glm::vec2(0.f, 0.f));
		m_connectorNormals.push_back(normal1);

		glm::vec3 normal2 = glm::cross(frontSecond, backSecond);
		m_connectorVertices.push_back(backFirst);
		m_connectorTextures.push_back(glm::vec2(1.f, 0.f));
		m_connectorNormals.push_back(normal2);

		m_connectorVertices.push_back(backSecond);
		m_connectorTextures.push_back(glm::vec2(1.f, 1.f));
		m_connectorNormals.push_back(normal2);


	}
	//For some reason, this triangle is skipped
	m_connectorVertices.push_back(m_frontVertices[0]);
	m_connectorTextures.push_back(glm::vec2(0.f, 1.f));
	m_connectorNormals.push_back(glm::cross(m_frontVertices[0], m_backVertices[0]));

	m_connectorVertices.push_back(m_frontVertices[1]);
	m_connectorTextures.push_back(glm::vec2(0.f, 0.f));
	m_connectorNormals.push_back(glm::cross(m_frontVertices[0], m_backVertices[0]));

	m_connectorVertices.push_back(m_backVertices[0]);
	m_connectorTextures.push_back(glm::vec2(1.f, 0.f));
	m_connectorNormals.push_back(glm::cross(m_frontVertices[1], m_backVertices[1]));

	m_connectorVertices.push_back(m_backVertices[1]);
	m_connectorTextures.push_back(glm::vec2(1.f, 1.f));
	m_connectorNormals.push_back(glm::cross(m_frontVertices[1], m_backVertices[1]));


	glGenVertexArrays(1, &m_connectorVao);
	glBindVertexArray(m_connectorVao);

	m_connectorVbo.Create();
	m_connectorVbo.Bind();

	for (int i = 0; i < m_connectorVertices.size(); i++)
	{
		m_connectorVbo.AddData(&m_connectorVertices[i], sizeof(glm::vec3));
		m_connectorVbo.AddData(&m_connectorTextures[i], sizeof(glm::vec2));
		m_connectorVbo.AddData(&m_connectorNormals[i], sizeof(glm::vec3));
	}


	m_connectorVbo.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//debug lines


	/*glGenVertexArrays(1, &d_lineVao);
	glBindVertexArray(d_lineVao);

	d_lineVbo.Create();
	d_lineVbo.Bind();

	for (int i = 0; i < d_linePoints.size(); i++)
	{
		d_lineVbo.AddData(&d_linePoints[i], sizeof(glm::vec3));
		d_lineVbo.AddData(&texCoords, sizeof(glm::vec2));
		d_lineVbo.AddData(&normalCoords, sizeof(glm::vec3));
	}


	d_lineVbo.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));*/
}




void CCoin::Render()
{
	glLineWidth(1.f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glDisable(GL_CULL_FACE);
	//glLineWidth(5.f);

	m_connectionTexture.Bind();
	glBindVertexArray(m_connectorVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_connectorVertices.size());
	glEnable(GL_CULL_FACE);
	m_faceTexture.Bind();
	glBindVertexArray(m_frontVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_frontVertices.size() + 1);
	//m_faceTexture.Release();
	glBindVertexArray(m_backVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_backVertices.size() + 1);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool CCoin::Collided(glm::vec3 otherPosition)
{
	if (glm::distance(otherPosition, m_position) <= 5.f)
	{
		return true;
	}
	return false;
}

void CCoin::Release()
{
	//TODO: release textures here
	m_faceTexture.Release();
	m_connectionTexture.Release();
	glDeleteVertexArrays(1, &m_frontVao);
	glDeleteVertexArrays(1, &m_backVao);
	glDeleteVertexArrays(1, &m_connectorVao);
	m_frontVbo.Release();
	m_backVbo.Release();
	m_connectorVbo.Release();
}

void CCoin::SetPosition(const glm::vec3 newPosition)
{
	m_position = newPosition;
}

glm::vec3 CCoin::Position()
{
	return m_position;
}

bool CCoin::isActive()
{
	return m_active;
}
void CCoin::SetActive(bool active)
{
	m_active = active;
}