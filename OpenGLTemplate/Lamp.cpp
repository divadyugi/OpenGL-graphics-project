#include "Lamp.h"

#define _USE_MATH_DEFINES

#include <math.h>

CLamp::CLamp() {}

CLamp::~CLamp()
{

}

int CLamp::Count()
{
	return m_count;
}

void CLamp::SetCount(int count)
{
	m_count = count;
}

glm::vec3 CLamp::Position()
{
	return m_position;
}

void CLamp::SetPosition(glm::vec3 newPosition)
{
	m_position = newPosition;
}

float CLamp::RotationAngle(glm::vec3 targetPoint)
{
	//Vector from top of pole to end of overhang
	glm::vec3 poleTop{ m_position.x, m_position.y + m_postHeight, m_position.z };
	glm::vec3 overhang_top_origin{ m_position.x+m_overhangLength,m_position.y+m_postHeight,m_position.z};
	glm::vec3 A = overhang_top_origin - poleTop;
	glm::vec3 B = targetPoint - m_position;
	float angle = glm::dot(A, B);
	angle = angle / (glm::length(A) * glm::length(B));
	return acos(angle);
}

void CLamp::Set(bool shining)
{
	m_shining = shining;
}

void CLamp::SetSize(float postRadius, float postHeight, float overhangRadius, float overhangLength, float coneBaseRadius, float coneLength, float coneEndRadius)
{
	m_postRadius = postRadius;
	m_postHeight = postHeight;
	m_overhangRadius = overhangRadius;
	m_overhangLength = overhangLength;
	m_coneBaseRadius = coneBaseRadius;
	m_coneLength = coneLength;
	m_coneEndRadius = coneEndRadius;
}

void CLamp::Create(int stacks)
{
	//Load in texture
	m_texture.Load("resources\\textures\\lamp_texture_rotated.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_R, GL_REPEAT);

	glGenVertexArrays(1, &m_postVao);
	glBindVertexArray(m_postVao);

	m_postVbo.Create();
	m_postVbo.Bind();

	//LAMP POST BASE

	//The base for the post of the lamp
	glm::vec3 post_origin{ 0.f,0.f,0.f };
	glm::vec2 texCoords{ 0.5f, 0.5f };
	glm::vec3 normalCoords{ 0.f, -1.f, 0.f };

	m_postVbo.AddData(&post_origin, sizeof(glm::vec3));
	m_postVbo.AddData(&texCoords, sizeof(glm::vec2));
	m_postVbo.AddData(&normalCoords, sizeof(glm::vec3));

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(cos(theta) * (float)m_postRadius, 0.f, sin(theta) * (float)m_postRadius);

		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };

		m_postBaseVertices.push_back(next);
		m_postVbo.AddData(&next, sizeof(glm::vec3));
		m_postVbo.AddData(&tex, sizeof(glm::vec2));
		m_postVbo.AddData(&normalCoords, sizeof(glm::vec3));
	}

	//LAMP POST TOP
	glm::vec3 post_top{ 0.f,m_postHeight, 0.f };
	glm::vec3 topNormalCoords{ 0.f, 1.f, 0.f };

	m_postVbo.AddData(&post_top, sizeof(glm::vec3));
	m_postVbo.AddData(&texCoords, sizeof(glm::vec2));
	m_postVbo.AddData(&normalCoords, sizeof(glm::vec3));

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(cos(theta) * (float)m_postRadius, m_postHeight, sin(theta) * (float)m_postRadius);
		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };

		m_postTopVertices.push_back(next);
		m_postTopTextures.push_back(tex);
	}

	for (int i = m_postTopVertices.size() - 1; i >= 0; i--)
	{
		m_postVbo.AddData(&m_postTopVertices[i], sizeof(glm::vec3));
		m_postVbo.AddData(&m_postTopTextures[i], sizeof(glm::vec2));
		m_postVbo.AddData(&topNormalCoords, sizeof(glm::vec3));
	}

	//LAMP POST CONNECTORS

	for (int i = 0; i < m_postBaseVertices.size(); i++)
	{
		glm::vec3 frontFirst{};
		glm::vec3 frontSecond{};
		glm::vec3 backFirst{};
		glm::vec3 backSecond{};
		if (i == m_postBaseVertices.size() - 1)
		{
			frontFirst = m_postBaseVertices.at(m_postBaseVertices.size() - 1);
			frontSecond = m_postBaseVertices.at(0);

			backFirst = m_postTopVertices.at(m_postTopVertices.size() - 1);
			backSecond = m_postTopVertices.at(0);
		}
		else
		{
			frontFirst = m_postBaseVertices.at(i);
			frontSecond = m_postBaseVertices.at(i + 1);

			backFirst = m_postTopVertices.at(i);
			backSecond = m_postTopVertices.at(i + 1);
		}
		m_postConnectorVertices.push_back(frontFirst);
		m_postConnectorTextures.push_back(glm::vec2(1.f, 0.f));

		m_postConnectorVertices.push_back(frontSecond);
		m_postConnectorTextures.push_back(glm::vec2(0.f, 0.f));

		m_postConnectorVertices.push_back(backFirst);
		m_postConnectorTextures.push_back(glm::vec2(0.f, 1.f));

		m_postConnectorVertices.push_back(backSecond);
		m_postConnectorTextures.push_back(glm::vec2(1.f, 1.f));
	}

	m_postConnectorVertices.push_back(m_postBaseVertices[0]);
	m_postConnectorTextures.push_back(glm::vec2(1.f, 0.f));

	m_postConnectorVertices.push_back(m_postBaseVertices[1]);
	m_postConnectorTextures.push_back(glm::vec2(0.f, 0.f));

	m_postConnectorVertices.push_back(m_postTopVertices[0]);
	m_postConnectorTextures.push_back(glm::vec2(0.f, 1.f));

	m_postConnectorVertices.push_back(m_postTopVertices[1]);
	m_postConnectorTextures.push_back(glm::vec2(1.f, 1.f));

	for (int i = 0; i < m_postConnectorVertices.size(); i++)
	{
		m_postVbo.AddData(&m_postConnectorVertices[i], sizeof(glm::vec3));
		m_postVbo.AddData(&m_postConnectorTextures[i], sizeof(glm::vec2));
		m_postVbo.AddData(&normalCoords, sizeof(glm::vec3));
	}

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);


	m_postVbo.UploadDataToGPU(GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	//Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//I could do all of these in one VAO, however that would not be as good for readability, do I sacrifice readability for performance?

	//I chose readability over performance for easier future maintenance
	//OVERHANG
	glGenVertexArrays(1, &m_overHangVao);
	glBindVertexArray(m_overHangVao);

	m_overHangVbo.Create();
	m_overHangVbo.Bind();


	//OVERHANG Base
	glm::vec3 overhang_post_origin{ 0.f, m_postHeight - m_overhangRadius, 0.f };
	glm::vec3 overhang_post_normal{ -1.f, 0.f, 0.f };

	m_overHangVbo.AddData(&overhang_post_origin, sizeof(glm::vec3));
	m_overHangVbo.AddData(&texCoords, sizeof(glm::vec2));
	m_overHangVbo.AddData(&overhang_post_normal, sizeof(glm::vec3));

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(0.f, cos(theta) * (float)m_overhangRadius + (m_postHeight - m_overhangRadius), sin(theta) * (float)m_overhangRadius);

		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };

		m_overhangBaseVertices.push_back(next);
		m_overHangVbo.AddData(&next, sizeof(glm::vec3));
		m_overHangVbo.AddData(&tex, sizeof(glm::vec2));
		m_overHangVbo.AddData(&normalCoords, sizeof(glm::vec3));
	}

	//OVERHANG TOP

	glm::vec3 overhang_top_origin{ m_overhangLength,m_postHeight - m_overhangRadius,0.f };
	glm::vec3 overhang_topNormalCoords{ 1.f,0.f,0.f };

	m_overHangVbo.AddData(&overhang_top_origin, sizeof(glm::vec3));
	m_overHangVbo.AddData(&texCoords, sizeof(glm::vec2));
	m_overHangVbo.AddData(&overhang_topNormalCoords, sizeof(glm::vec3));

	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3(m_overhangLength, cos(theta) * (float)m_overhangRadius + (m_postHeight - m_overhangRadius), sin(theta) * (float)m_overhangRadius);
		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };

		m_overhangTopVertices.push_back(next);
		m_overhangTopTextures.push_back(tex);

		/*m_topVbo.AddData(&next, sizeof(glm::vec3));
		m_topVbo.AddData(&texCoords, sizeof(glm::vec2));
		m_topVbo.AddData(&normalCoords, sizeof(glm::vec3));*/
	}
	for (int i = m_overhangTopVertices.size() - 1; i >= 0; i--)
	{
		m_overHangVbo.AddData(&m_overhangTopVertices[i], sizeof(glm::vec3));
		m_overHangVbo.AddData(&m_overhangTopTextures[i], sizeof(glm::vec2));
		m_overHangVbo.AddData(&topNormalCoords, sizeof(glm::vec3));
	}


	//OVERHANG CONNECTORS

	for (int i = 0; i < m_overhangBaseVertices.size(); i++)
	{
		glm::vec3 frontFirst{};
		glm::vec3 frontSecond{};
		glm::vec3 backFirst{};
		glm::vec3 backSecond{};
		if (i == m_overhangBaseVertices.size() - 1)
		{
			frontFirst = m_overhangBaseVertices.at(m_overhangBaseVertices.size() - 1);
			frontSecond = m_overhangBaseVertices.at(0);

			backFirst = m_overhangTopVertices.at(m_overhangTopVertices.size() - 1);
			backSecond = m_overhangTopVertices.at(0);
		}
		else
		{
			frontFirst = m_overhangBaseVertices.at(i);
			frontSecond = m_overhangBaseVertices.at(i + 1);

			backFirst = m_overhangTopVertices.at(i);
			backSecond = m_overhangTopVertices.at(i + 1);
		}
		m_overhangConnectorVertices.push_back(frontFirst);
		m_overhangConnectorTextures.push_back(glm::vec2(1.f, 0.f));

		m_overhangConnectorVertices.push_back(frontSecond);
		m_overhangConnectorTextures.push_back(glm::vec2(0.f, 0.f));

		m_overhangConnectorVertices.push_back(backFirst);
		m_overhangConnectorTextures.push_back(glm::vec2(0.f, 1.f));

		m_overhangConnectorVertices.push_back(backSecond);
		m_overhangConnectorTextures.push_back(glm::vec2(1.f, 1.f));
	}

	m_overhangConnectorVertices.push_back(m_overhangBaseVertices[0]);
	m_overhangConnectorTextures.push_back(glm::vec2(1.f, 0.f));

	m_overhangConnectorVertices.push_back(m_overhangBaseVertices[1]);
	m_overhangConnectorTextures.push_back(glm::vec2(0.f, 0.f));

	m_overhangConnectorVertices.push_back(m_overhangTopVertices[0]);
	m_overhangConnectorTextures.push_back(glm::vec2(0.f, 1.f));

	m_overhangConnectorVertices.push_back(m_overhangTopVertices[1]);
	m_overhangConnectorTextures.push_back(glm::vec2(1.f, 1.f));

	for (int i = 0; i < m_overhangConnectorVertices.size(); i++)
	{
		m_overHangVbo.AddData(&m_overhangConnectorVertices[i], sizeof(glm::vec3));
		m_overHangVbo.AddData(&m_overhangConnectorTextures[i], sizeof(glm::vec2));
		m_overHangVbo.AddData(&normalCoords, sizeof(glm::vec3));
	}



	m_overHangVbo.UploadDataToGPU(GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	//Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	//CONE
	glGenVertexArrays(1, &m_coneVao);
	glBindVertexArray(m_coneVao);

	m_coneVbo.Create();
	m_coneVbo.Bind();


	glm::vec3 cone_base_origin{ m_overhangLength - m_coneBaseRadius, (m_postHeight - m_overhangRadius), 0.f };
	glm::vec3 cone_base_normalCoords{ 0.f, 1.f, 0.f };

	m_coneVbo.AddData(&cone_base_origin, sizeof(glm::vec3));
	m_coneVbo.AddData(&texCoords, sizeof(glm::vec2));
	m_coneVbo.AddData(&cone_base_normalCoords, sizeof(glm::vec3));


	for (unsigned int i = 0; i <= stacks; i++)
	{
		float div = i / (float)stacks;
		float theta = div * 2 * (float)M_PI;

		glm::vec3 next = glm::vec3((m_overhangLength - m_coneBaseRadius) + cos(theta) * (float)m_coneBaseRadius, (m_postHeight - m_overhangRadius * 2) - m_coneLength, sin(theta) * (float)m_coneBaseRadius);
		glm::vec2 tex{ ((1 + cos(theta)) / 2), ((1 + sin(theta)) / 2) };


		m_coneTopVertices.push_back(next);
		m_coneTopTextures.push_back(tex);
	}

	for (int i = m_coneTopVertices.size() - 1; i >= 0; i--)
	{
		m_coneVbo.AddData(&m_coneTopVertices[i], sizeof(glm::vec3));
		m_coneVbo.AddData(&m_coneTopTextures[i], sizeof(glm::vec2));
		m_coneVbo.AddData(&cone_base_normalCoords, sizeof(glm::vec3));
	}



	m_coneVbo.UploadDataToGPU(GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	//Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	//Normal coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));




}

void CLamp::Render()
{
	/*//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
	glLineWidth(5.f);

	m_texture.Bind();
	//LAMP POST BASE
	glBindVertexArray(m_postVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_postBaseVertices.size() + 1);
	//LAMP POST TOP
	//+2 to base vertex size if the last element on the previous is included, +1 if it is not.
	glDrawArrays(GL_TRIANGLE_FAN, m_postBaseVertices.size() + 1, m_postTopVertices.size() + 1, 2);

	//LAMP POST CONNECTOR
	glDrawArrays(GL_TRIANGLE_STRIP, m_postTopVertices.size() + m_postBaseVertices.size() + 2, m_postConnectorVertices.size());

	//OVERHANG BASE
	glBindVertexArray(m_overHangVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_overhangBaseVertices.size() + 1);

	//OVERHANG TOP
	glBindVertexArray(m_overHangVao);
	glDrawArrays(GL_TRIANGLE_FAN, m_overhangBaseVertices.size() + 1, m_overhangTopVertices.size() + 1);

	//OVERHANG CONNECTOR
	glDrawArrays(GL_TRIANGLE_STRIP, m_overhangBaseVertices.size() + m_overhangTopVertices.size() + 2, m_overhangConnectorVertices.size());

	//Cone Top
	glBindVertexArray(m_coneVao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, m_coneTopVertices.size() + 1);

	glEnable(GL_CULL_FACE);*/
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glLineWidth(5.f);

	m_texture.Bind();
	//LAMP POST BASE
	glBindVertexArray(m_postVao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_postBaseVertices.size() + 1, 25);
	//LAMP POST TOP
	//+2 to base vertex size if the last element on the previous is included, +1 if it is not.
	glDrawArraysInstanced(GL_TRIANGLE_FAN, m_postBaseVertices.size() + 1, m_postTopVertices.size() + 1, 25);

	//LAMP POST CONNECTOR
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, m_postTopVertices.size() + m_postBaseVertices.size() + 2, m_postConnectorVertices.size(), 25);

	//OVERHANG BASE
	glBindVertexArray(m_overHangVao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_overhangBaseVertices.size() + 1, 25);

	//OVERHANG TOP
	glBindVertexArray(m_overHangVao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, m_overhangBaseVertices.size() + 1, m_overhangTopVertices.size() + 1, 25);

	//OVERHANG CONNECTOR
	glDrawArraysInstanced(GL_TRIANGLE_STRIP, m_overhangBaseVertices.size() + m_overhangTopVertices.size() + 2, m_overhangConnectorVertices.size(), 25);

	//Cone Top
	glBindVertexArray(m_coneVao);
	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, m_coneTopVertices.size() + 1, 25);

	glEnable(GL_CULL_FACE);

}