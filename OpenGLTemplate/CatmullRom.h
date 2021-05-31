#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "Texture.h"

class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	//Render and create the centreline of the track
	void CreateCentreline(int N);
	void RenderCentreline();

	//Offset curves used for the edges of the track
	void CreateOffsetCurves();
	void RenderOffsetCurves();

	//Creation of the track triangles using the offsets
	void CreateTrack();
	void RenderTrack();

	//Offroad on the right side of the track
	void CreateRightOffRoad();
	void CreateLeftOffRoad();
	void RenderRightOffRoad();
	void RenderLeftOffRoad();

	//Rail creation on the right side of the track
	void CreateRightRails();
	void CreateLeftRails();
	void RenderRightRails();
	void RenderLeftRails();

	//Returns the number of laps taken around the track, based ond istance from the startPoint;
	int CurrentLap(int d, int lapCount);

	void SetHeight(float height);

	//Returns a point on the centreline d distance along the control curve
	bool Sample(float d, glm::vec3& p, glm::vec3& up = _dummy_vector);

	void Release();

	bool isOffRoading(const glm::vec3 actualPosition, const int expectedPosition);

	bool hasCollided(const glm::vec3& position, const int expectedPosition);

	glm::vec3 RandomPoint();

	int ClosestPoint(glm::vec3 point);

	glm::vec3 LampPosition(int point);

	glm::vec3 CenterPoint(int position);

private:
	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t);
	float w = 20.f;

	//Centreline
	std::vector<float> m_distances;
	std::vector<glm::vec3> m_controlPoints;
	std::vector <glm::vec3> m_controlUpVectors;
	std::vector<glm::vec3> m_centrelinePoints;
	std::vector<glm::vec3> m_centrelineUpVectors;

	//Centreline VAOs and VBOs
	GLuint m_vaoCentreline;
	CVertexBufferObject m_vboCentreline;

	//Rails:
	CTexture m_railTexture;
	float m_railHeight;
	//Left offset (rails)
	std::vector<glm::vec3> m_leftOffsetPoints;
	std::vector<glm::vec3> m_leftRailPoints;

	//Right offset (rails)
	std::vector<glm::vec3> m_rightOffsetPoints;
	std::vector<glm::vec3> m_rightRailPoints;

	//Offset VAO and VBO (debugging)
	//These are used for debugging, could use only one vao and vbo for these if wanted
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	CVertexBufferObject m_vboLeftOffsetCurve;
	CVertexBufferObject m_vboRightOffsetCurve;

	//Rail VAOs and VBOs
	//Again, could use single VAO and VBO
	//Left rail:
	GLuint m_vaoLeftRail;
	GLuint m_vaoLeftRailFace;
	CVertexBufferObject m_vboLeftRail;
	CVertexBufferObject m_vboLeftRailFace;
	std::vector<glm::vec3> m_leftRailTriangles;
	std::vector<glm::vec2> m_leftRailextures;

	//Right rail

	GLuint m_vaoRightRail;
	GLuint m_vaoRightRailFace;
	CVertexBufferObject m_vboRightRail;
	CVertexBufferObject m_vboRightRailFace;
	std::vector<glm::vec3> m_rightRailTriangles;
	std::vector<glm::vec2> m_rightRailTextures;


	//Offroad offset
	CTexture m_offRoadTexture;
	float m_offroadWidth = 60.f;

	//Right offset:
	std::vector<glm::vec3> m_rightOffroad;
	std::vector<glm::vec3> m_rightOffroadVertices;
	std::vector<glm::vec2> m_rightOffroadTexture;
	GLuint m_vaoRightOffroad;
	CVertexBufferObject m_vboRightOffroad;
	GLuint m_vaoRightOffroadFace;
	CVertexBufferObject m_vboRightOffroadFace;

	//Left offset:
	std::vector<glm::vec3> m_leftOffroad;
	std::vector<glm::vec3> m_leftOffroadVertices;
	std::vector<glm::vec2> m_leftOffroadTexture;
	GLuint m_vaoLeftOffroad;
	CVertexBufferObject m_vboLeftOffroad;
	GLuint m_vaoLeftOffroadFace;
	CVertexBufferObject m_vboLeftOffroadFace;

	//Track
	CTexture m_texture;
	std::vector<glm::vec3> m_triangleVertices;
	std::vector<glm::vec2> m_trackTextures;
	//Track VAOs and VBOs
	GLuint m_vaoTrack;
	CVertexBufferObject m_vboTrack;


	//Number of vertices in track VBO
	unsigned int m_vertexCount;


	static glm::vec3 _dummy_vector;
};