#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>

CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
}

CCatmullRom::~CCatmullRom()
{
	//Release();
}

int CCatmullRom::CurrentLap(int d, int lapCount)
{
	if (d >= 4000 * lapCount)
	{
		return 1;
	}
	return 0;
}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f * p1 - 3.0f * p2 + p3);

	return a + b * t + c * t2 + d * t3;

}

//Render lamps at point p, with offset N
glm::vec3 CCatmullRom::LampPosition(int point)
{
	//Place lamp at point p
	//Then offset it by (width+offRoadWidth)/2 +small amount so that it is outside the rails.
	glm::vec3 p{};
	glm::vec3 pNext{};
	if (point == m_centrelinePoints.size() - 1)
	{
		p = m_centrelinePoints.at(point);
		pNext = m_centrelinePoints.at(0);
	}
	else
	{
		p = m_centrelinePoints.at(point);

		pNext = m_centrelinePoints.at(point + 1);
	}

	glm::vec3 T = glm::normalize(pNext - p);
	glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 B = glm::normalize(N * T);

	return (p + ((m_offroadWidth / 2) + 5.f) * N);
}

void CCatmullRom::SetHeight(float height)
{
	m_railHeight = height;
}

int CCatmullRom::ClosestPoint(glm::vec3 point)
{
	int maxDist = INT_MAX;
	int pos = 0;
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		float dist = glm::distance(point, m_centrelinePoints[i]);
		if (dist <= maxDist)
		{
			pos = i;
			maxDist = dist;
		}
	}
	return pos;
}

//Compare the distance between the position he is on
bool CCatmullRom::isOffRoading(const glm::vec3 actualPosition, const int expectedPosition)
{
	if (glm::distance(actualPosition, m_centrelinePoints[expectedPosition]) >= 15)
	{
		return true;
	}

	return false;
}

bool CCatmullRom::hasCollided(const glm::vec3& position, const int expectedPosition)
{
	if (glm::distance(position, m_centrelinePoints[expectedPosition]) >= 30)
	{
		return true;
	}

	return false;
}

glm::vec3 CCatmullRom::CenterPoint(int position)
{
	return m_centrelinePoints[position];
}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk

	//When moving the camera along the B-axis of the TNB frame, there has to be a slight elevation (y-axis) change in one of the points, otherwise it won't work
	m_controlPoints.push_back(glm::vec3(0.f, 50.f, 95.f));
	m_controlPoints.push_back(glm::vec3(180.f, 51.f, 90.f));
	m_controlPoints.push_back(glm::vec3(250.f, 52.f, 130.f));
	m_controlPoints.push_back(glm::vec3(250.f, 51.f, 180.f));
	m_controlPoints.push_back(glm::vec3(180.f, 52.f, 200.f));
	m_controlPoints.push_back(glm::vec3(50.f, 51.f, 180.f));
	m_controlPoints.push_back(glm::vec3(137.5f, 52.f, 450.f));
	m_controlPoints.push_back(glm::vec3(225.f, 51.f, 540.f));
	m_controlPoints.push_back(glm::vec3(350.f, 52.f, 405.f));
	m_controlPoints.push_back(glm::vec3(450.f, 51.f, 450.f));
	m_controlPoints.push_back(glm::vec3(475.f, 52.f, 495.f));
	m_controlPoints.push_back(glm::vec3(550.f, 51.f, 540.f));
	m_controlPoints.push_back(glm::vec3(600.f, 52.f, 610.f));
	m_controlPoints.push_back(glm::vec3(650.f, 51.f, 610.f));
	m_controlPoints.push_back(glm::vec3(800.f, 70.f, 590.f));
	m_controlPoints.push_back(glm::vec3(900.f, 75.f, 562.5f));
	m_controlPoints.push_back(glm::vec3(1000.f, 76.f, 562.5f));
	m_controlPoints.push_back(glm::vec3(1075.f, 77.f, 472.5f));
	m_controlPoints.push_back(glm::vec3(1000.f, 76.f, 405.f));
	m_controlPoints.push_back(glm::vec3(900.f, 77.f, 405.f));
	m_controlPoints.push_back(glm::vec3(850.f, 76.f, 337.5f));
	m_controlPoints.push_back(glm::vec3(900.f, 77.f, 270.f));
	m_controlPoints.push_back(glm::vec3(1000.f, 76.f, 270.f));
	m_controlPoints.push_back(glm::vec3(1100.f, 77.f, 270.f));
	m_controlPoints.push_back(glm::vec3(1175.f, 65.f, 180.f));
	m_controlPoints.push_back(glm::vec3(1100.f, 55.f, 90.f));
	m_controlPoints.push_back(glm::vec3(500.f, 15.f, -25.f));
	m_controlPoints.push_back(glm::vec3(-50.f, 50.f, -25.f));

	/*m_controlPoints.push_back(glm::vec3(1000.f, 76.f, 135.f));
	m_controlPoints.push_back(glm::vec3(900.f, 77.f, 135.f));
	m_controlPoints.push_back(glm::vec3(850.f, 76.f, 67.5f));*/
	//m_controlPoints.push_back(glm::vec3(900.f, 77.f, 0.f));


	/*m_controlPoints.push_back(glm::vec3(81, 5, -71));
	m_controlPoints.push_back(glm::vec3(91, 8, -71));
	m_controlPoints.push_back(glm::vec3(86, 10, -71));
	m_controlPoints.push_back(glm::vec3(78.5f, 10, -71));
	m_controlPoints.push_back(glm::vec3(76, 8, -71));
	m_controlPoints.push_back(glm::vec3(78.5f, 6.5f, -71));
	m_controlPoints.push_back(glm::vec3(93, 5, -85));*/


	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(1.f, 0.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(1.f, 0.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(1.f, 0.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));
	m_controlUpVectors.push_back(glm::vec3(0.f, 1.f, 0.f));

}

// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();


	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3& p, glm::vec3& up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size() - 1; i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));

	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}

//Creation

//Centreline - used for debugging

//If point == m_centrelinePoints.sizE()...
//Otherwise just work out TNB frame normally, and check if distance given is > p+(width/2)*N or < p-(w/2)*N, if yes, then he is offroading
//You can also check if he collided with rail by inputting offroadWidth instead of width

void CCatmullRom::CreateCentreline(int N)
{
	//Set the control points
	SetControlPoints();

	//Uniformly sample the control points
	//N is the number of points we want the line to have
	UniformlySampleControlPoints(N);

	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);

	m_vboCentreline.Create();
	m_vboCentreline.Bind();

	//This is just going to be the line in the middle, don't even need to render actaully, only render for debugging.
	glm::vec2 texCoord(0.f, 0.f);
	glm::vec3 normalCoord(0.f, 1.f, 0.f);

	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		m_vboCentreline.AddData(&m_centrelinePoints.at(i), sizeof(glm::vec3));
		m_vboCentreline.AddData(&texCoord, sizeof(glm::vec2));
		m_vboCentreline.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboCentreline.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

//First offset curves, used for the rails
void CCatmullRom::CreateOffsetCurves()
{
	glm::vec3 T_temp{};
	glm::vec3 N_temp{};
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 p{};
		glm::vec3 pNext{};
		if (i == m_centrelinePoints.size() - 1)
		{
			p = m_centrelinePoints.at(i);
			pNext = m_centrelinePoints.at(0);
		}
		else
		{
			p = m_centrelinePoints.at(i);

			pNext = m_centrelinePoints.at(i + 1);
		}

		glm::vec3 T = glm::normalize(pNext - p);
		glm::vec3 N = glm::normalize(glm::cross(T, glm::vec3(0.f, 1.f, 0.f)));
		glm::vec3 B = glm::normalize(N * T);

		glm::vec3 l = p - (w / 2) * N;
		glm::vec3 r = p + (w / 2) * N;

		m_leftOffsetPoints.push_back(l);
		m_rightOffsetPoints.push_back(r);

		//Offset
		glm::vec3 offroadL = p - (m_offroadWidth / 2) * N;
		glm::vec3 offroadR = p + (m_offroadWidth / 2) * N;

		m_rightOffroad.push_back(offroadR);
		m_leftOffroad.push_back(offroadL);



		//Create the side walls of the track here
		glm::vec3 rail_left = offroadL + m_railHeight * glm::vec3(0.f, 1.f, 0.f);
		glm::vec3 rail_right = offroadR + m_railHeight * glm::vec3(0.f, 1.f, 0.f);
		m_leftRailPoints.push_back(rail_left);
		m_rightRailPoints.push_back(rail_right);
	}

	// Generate two VAOs called m_vaoLeftOffsetCurve and m_vaoRightOffsetCurve, each with a VBO, and get the offset curve points on the graphics card
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);

	m_vboLeftOffsetCurve.Create();
	m_vboLeftOffsetCurve.Bind();

	glm::vec2 texCoord(0.f, 0.f);
	glm::vec3 normalCoord(0.f, 1.f, 0.f);

	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		m_vboLeftOffsetCurve.AddData(&m_leftOffsetPoints.at(i), sizeof(glm::vec3));
		m_vboLeftOffsetCurve.AddData(&texCoord, sizeof(glm::vec2));
		m_vboLeftOffsetCurve.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboLeftOffsetCurve.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	// Note it is possible to only use one VAO / VBO with all the points instead.

	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);

	m_vboRightOffsetCurve.Create();
	m_vboRightOffsetCurve.Bind();

	for (int i = 0; i < m_rightOffsetPoints.size(); i++)
	{
		m_vboRightOffsetCurve.AddData(&m_rightOffsetPoints.at(i), sizeof(glm::vec3));
		m_vboRightOffsetCurve.AddData(&texCoord, sizeof(glm::vec2));
		m_vboRightOffsetCurve.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboRightOffsetCurve.UploadDataToGPU(GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

//Right rails
void CCatmullRom::CreateRightRails()
{


	glGenVertexArrays(1, &m_vaoRightRail);
	glBindVertexArray(m_vaoRightRail);

	m_vboRightRail.Create();
	m_vboRightRail.Bind();

	glm::vec2 texCoord{ 0.f,0.f };

	glm::vec3 normalCoordRight{ -1.f,0.f,0.f };

	for (int i = 0; i < m_rightRailPoints.size(); i++)
	{
		m_vboRightRail.AddData(&m_rightRailPoints.at(i), sizeof(glm::vec3));
		m_vboRightRail.AddData(&texCoord, sizeof(glm::vec2));
		m_vboRightRail.AddData(&normalCoordRight, sizeof(glm::vec3));
	}

	m_vboRightRail.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	//Face creation

	//Texture settings
	m_railTexture.Load("resources\\textures\\Rail_texture.png");
	m_railTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_railTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_railTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_railTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Faces
	for (int i = 0; i < m_rightRailPoints.size(); i++)
	{
		//Right values are from the rail
		//Left values are from the rightOffset
		glm::vec3 pLeft{};
		glm::vec3 pRight{};
		glm::vec3 pNextLeft{};
		glm::vec3 pNextRight{};

		if (i == m_rightRailPoints.size() - 1)
		{
			pLeft = m_rightOffroad.at(i);
			pNextLeft = m_rightOffroad.at(0);
			pRight = m_rightRailPoints.at(i);
			pNextRight = m_rightRailPoints.at(0);
		}
		else
		{
			pLeft = m_rightOffroad.at(i);
			pNextLeft = m_rightOffroad.at(i + 1);

			pRight = m_rightRailPoints.at(i);
			pNextRight = m_rightRailPoints.at(i + 1);
		}

		m_rightRailTriangles.push_back(pLeft);
		m_rightRailTextures.push_back(glm::vec2(0.f, 0.f));
		m_rightRailTriangles.push_back(pRight);
		m_rightRailTextures.push_back(glm::vec2(0.f, 1.f));
		m_rightRailTriangles.push_back(pNextLeft);
		m_rightRailTextures.push_back(glm::vec2(1.f, 0.f));

		m_rightRailTriangles.push_back(pNextRight);
		m_rightRailTextures.push_back(glm::vec2(1.f, 1.f));
	}

	glGenVertexArrays(1, &m_vaoRightRailFace);
	glBindVertexArray(m_vaoRightRailFace);

	m_vboRightRailFace.Create();
	m_vboRightRailFace.Bind();

	for (int i = 0; i < m_rightRailTriangles.size(); i++)
	{
		m_vboRightRailFace.AddData(&m_rightRailTriangles.at(i), sizeof(glm::vec3));
		m_vboRightRailFace.AddData(&m_rightRailTextures.at(i), sizeof(glm::vec2));
		m_vboRightRailFace.AddData(&normalCoordRight, sizeof(glm::vec3));
	}

	m_vboRightRailFace.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


}

void CCatmullRom::CreateLeftRails()
{
	glGenVertexArrays(1, &m_vaoLeftRail);
	glBindVertexArray(m_vaoLeftRail);

	m_vboLeftRail.Create();
	m_vboLeftRail.Bind();

	glm::vec2 texCoord{ 0.f, 0.f };
	glm::vec3 normalCoord{ 1.f, 0.f, 0.f };

	for (int i = 0; i < m_leftRailPoints.size(); i++)
	{
		m_vboLeftRail.AddData(&m_leftRailPoints.at(i), sizeof(glm::vec3));
		m_vboLeftRail.AddData(&texCoord, sizeof(glm::vec2));
		m_vboLeftRail.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboLeftRail.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	//FACE
	for (int i = 0; i < m_leftRailPoints.size(); i++)
	{
		//Right values are from the rail
		//Left values are from the rightOffset
		glm::vec3 pLeft{};
		glm::vec3 pRight{};
		glm::vec3 pNextLeft{};
		glm::vec3 pNextRight{};

		if (i == m_leftRailPoints.size() - 1)
		{
			pLeft = m_leftOffroad.at(i);
			pNextLeft = m_leftOffroad.at(0);
			pRight = m_leftRailPoints.at(i);
			pNextRight = m_leftRailPoints.at(0);
		}
		else
		{
			pLeft = m_leftOffroad.at(i);
			pNextLeft = m_leftOffroad.at(i + 1);

			pRight = m_leftRailPoints.at(i);
			pNextRight = m_leftRailPoints.at(i + 1);
		}

		m_leftRailTriangles.push_back(pLeft);
		m_leftRailextures.push_back(glm::vec2(0.f, 0.f));
		m_leftRailTriangles.push_back(pNextLeft);
		m_leftRailextures.push_back(glm::vec2(1.f, 0.f));
		m_leftRailTriangles.push_back(pRight);
		m_leftRailextures.push_back(glm::vec2(0.f, 1.f));

		m_leftRailTriangles.push_back(pRight);
		m_leftRailextures.push_back(glm::vec2(0.f, 1.f));
		m_leftRailTriangles.push_back(pNextLeft);
		m_leftRailextures.push_back(glm::vec2(1.f, 0.f));
		m_leftRailTriangles.push_back(pNextRight);
		m_leftRailextures.push_back(glm::vec2(1.f, 1.f));

	}

	glGenVertexArrays(1, &m_vaoLeftRailFace);
	glBindVertexArray(m_vaoLeftRailFace);

	m_vboLeftRailFace.Create();
	m_vboLeftRailFace.Bind();

	for (int i = 0; i < m_leftRailTriangles.size(); i++)
	{
		m_vboLeftRailFace.AddData(&m_leftRailTriangles.at(i), sizeof(glm::vec3));
		m_vboLeftRailFace.AddData(&m_leftRailextures.at(i), sizeof(glm::vec2));
		m_vboLeftRailFace.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboLeftRailFace.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::CreateTrack()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	//We need to store them in the order of
	//p left, p right, pnext left
	//pnext left, p right, pnext right
	m_texture.Load("resources\\textures\\Road_texture.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		glm::vec3 pLeft{};
		glm::vec3 pRight{};
		glm::vec3 pNextLeft{};
		glm::vec3 pNextRight{};
		if (i == m_centrelinePoints.size() - 1)
		{
			pLeft = m_leftOffsetPoints.at(m_centrelinePoints.size() - 1);
			pNextLeft = m_leftOffsetPoints.at(0);
			pRight = m_rightOffsetPoints.at(m_centrelinePoints.size() - 1);
			pNextRight = m_rightOffsetPoints.at(0);
		}
		else
		{
			pLeft = m_leftOffsetPoints.at(i);
			pNextLeft = m_leftOffsetPoints.at(i + 1);

			pRight = m_rightOffsetPoints.at(i);
			pNextRight = m_rightOffsetPoints.at(i + 1);
		}

		m_triangleVertices.push_back(pLeft);
		m_trackTextures.push_back(glm::vec2(0.f, 1.f));
		m_triangleVertices.push_back(pRight);
		m_trackTextures.push_back(glm::vec2(1.f, 1.f));
		m_triangleVertices.push_back(pNextLeft);
		m_trackTextures.push_back(glm::vec2(0.f, 0.f));
		//pNextLeft repeats...maybe we can simplify it?
		/*m_triangleVertices.push_back(pNextLeft);
		m_trackTextures.push_back(glm::vec2(1.f, 0.f));*/
		/*m_triangleVertices.push_back(pRight);
		m_trackTextures.push_back(glm::vec2(0.f, 1.f));*/
		m_triangleVertices.push_back(pNextRight);
		m_trackTextures.push_back(glm::vec2(1.f, 0.f));

	}

	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);

	m_vboTrack.Create();
	m_vboTrack.Bind();

	glm::vec2 texCoord{ 0.f,0.f };
	glm::vec3 normalCoord{ 0.f, 1.f, 0.f };

	for (int i = 0; i < m_triangleVertices.size(); i++)
	{
		m_vboTrack.AddData(&m_triangleVertices.at(i), sizeof(glm::vec3));
		m_vboTrack.AddData(&m_trackTextures.at(i), sizeof(glm::vec2));
		m_vboTrack.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboTrack.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = 2 * (sizeof(glm::vec3)) + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));



}


void CCatmullRom::CreateRightOffRoad()
{
	/*DEBUGGING*/
	glGenVertexArrays(1, &m_vaoRightOffroad);
	glBindVertexArray(m_vaoRightOffroad);

	m_vboRightOffroad.Create();
	m_vboRightOffroad.Bind();

	glm::vec2 texCoord{ 0.f, 0.f };

	glm::vec3 normalCoord{ 0.f,1.f,0.f };
	for (int i = 0; i < m_rightOffroad.size(); i++)
	{
		m_vboRightOffroad.AddData(&m_rightOffroad.at(i), sizeof(glm::vec3));
		m_vboRightOffroad.AddData(&texCoord, sizeof(glm::vec2));
		m_vboRightOffroad.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboRightOffroad.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));


	/*FACE*/

	//Texture from https://opengameart.org/content/seamless-dirt-64x64
	m_offRoadTexture.Load("resources\\textures\\dirt_texture.png");
	m_offRoadTexture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_offRoadTexture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_offRoadTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_offRoadTexture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Faces
	for (int i = 0; i < m_rightRailPoints.size(); i++)
	{
		//Right values are from the rail
		//Left values are from the rightOffset
		glm::vec3 pLeft{};
		glm::vec3 pRight{};
		glm::vec3 pNextLeft{};
		glm::vec3 pNextRight{};

		if (i == m_rightRailPoints.size() - 1)
		{
			pLeft = m_rightOffsetPoints.at(i);
			pNextLeft = m_rightOffsetPoints.at(0);
			pRight = m_rightOffroad.at(i);
			pNextRight = m_rightOffroad.at(0);
		}
		else
		{
			pLeft = m_rightOffsetPoints.at(i);
			pNextLeft = m_rightOffsetPoints.at(i + 1);

			pRight = m_rightOffroad.at(i);
			pNextRight = m_rightOffroad.at(i + 1);
		}

		m_rightOffroadVertices.push_back(pLeft);
		m_rightOffroadTexture.push_back(glm::vec2(0.f, 0.f));
		m_rightOffroadVertices.push_back(pRight);
		m_rightOffroadTexture.push_back(glm::vec2(0.f, 1.f));
		m_rightOffroadVertices.push_back(pNextLeft);
		m_rightOffroadTexture.push_back(glm::vec2(1.f, 0.f));

		//m_rightRailTriangles.push_back(pNextLeft);
		//m_rightRailTriangles.push_back(pRight);
		m_rightOffroadVertices.push_back(pNextRight);
		m_rightOffroadTexture.push_back(glm::vec2(1.f, 1.f));
	}

	glGenVertexArrays(1, &m_vaoRightOffroadFace);
	glBindVertexArray(m_vaoRightOffroadFace);

	m_vboRightOffroadFace.Create();
	m_vboRightOffroadFace.Bind();

	for (int i = 0; i < m_rightRailTriangles.size(); i++)
	{
		m_vboRightOffroadFace.AddData(&m_rightOffroadVertices.at(i), sizeof(glm::vec3));
		m_vboRightOffroadFace.AddData(&m_rightOffroadTexture.at(i), sizeof(glm::vec2));
		m_vboRightOffroadFace.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboRightOffroadFace.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}


void CCatmullRom::CreateLeftOffRoad()
{
	glGenVertexArrays(1, &m_vaoLeftOffroad);
	glBindVertexArray(m_vaoLeftOffroad);

	m_vboLeftOffroad.Create();
	m_vboLeftOffroad.Bind();

	glm::vec2 texCoord{ 0.f, 0.f };

	glm::vec3 normalCoord{ 0.f,1.f,0.f };
	for (int i = 0; i < m_leftOffroad.size(); i++)
	{
		m_vboLeftOffroad.AddData(&m_leftOffroad.at(i), sizeof(glm::vec3));
		m_vboLeftOffroad.AddData(&texCoord, sizeof(glm::vec2));
		m_vboLeftOffroad.AddData(&normalCoord, sizeof(glm::vec3));
	}

	for (int i = 0; i < m_leftRailPoints.size(); i++)
	{
		//Right values are from the rail
		//Left values are from the rightOffset
		glm::vec3 pLeft{};
		glm::vec3 pRight{};
		glm::vec3 pNextLeft{};
		glm::vec3 pNextRight{};

		if (i == m_leftRailPoints.size() - 1)
		{
			pLeft = m_leftOffsetPoints.at(i);
			pNextLeft = m_leftOffsetPoints.at(0);
			pRight = m_leftOffroad.at(i);
			pNextRight = m_leftOffroad.at(0);
		}
		else
		{
			pLeft = m_leftOffsetPoints.at(i);
			pNextLeft = m_leftOffsetPoints.at(i + 1);

			pRight = m_leftOffroad.at(i);
			pNextRight = m_leftOffroad.at(i + 1);
		}

		m_leftOffroadVertices.push_back(pLeft);
		m_leftOffroadTexture.push_back(glm::vec2(0.f, 0.f));
		m_leftOffroadVertices.push_back(pNextLeft);
		m_leftOffroadTexture.push_back(glm::vec2(1.f, 0.f));
		m_leftOffroadVertices.push_back(pRight);
		m_leftOffroadTexture.push_back(glm::vec2(0.f, 1.f));

		m_leftOffroadVertices.push_back(pRight);
		m_leftOffroadTexture.push_back(glm::vec2(0.f, 1.f));
		m_leftOffroadVertices.push_back(pNextLeft);
		m_leftOffroadTexture.push_back(glm::vec2(1.f, 0.f));
		m_leftOffroadVertices.push_back(pNextRight);
		m_leftOffroadTexture.push_back(glm::vec2(1.f, 1.f));
	}

	m_vboLeftOffroad.UploadDataToGPU(GL_STATIC_DRAW);

	GLsizei istride = sizeof(glm::vec3) * 2 + sizeof(glm::vec2);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));




	glGenVertexArrays(1, &m_vaoLeftOffroadFace);
	glBindVertexArray(m_vaoLeftOffroadFace);

	m_vboLeftOffroadFace.Create();
	m_vboLeftOffroadFace.Bind();

	for (int i = 0; i < m_leftOffroadVertices.size(); i++)
	{
		m_vboLeftOffroadFace.AddData(&m_leftOffroadVertices.at(i), sizeof(glm::vec3));
		m_vboLeftOffroadFace.AddData(&m_leftOffroadTexture.at(i), sizeof(glm::vec2));
		m_vboLeftOffroadFace.AddData(&normalCoord, sizeof(glm::vec3));
	}

	m_vboLeftOffroadFace.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, istride, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, istride, (void*)sizeof(glm::vec3));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, istride, (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void CCatmullRom::RenderLeftOffRoad()
{
	glBindVertexArray(m_vaoLeftOffroad);
	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffroad.size());

	//glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	m_offRoadTexture.Bind();
	glBindVertexArray(m_vaoLeftOffroadFace);
	glDrawArrays(GL_TRIANGLES, 0, m_leftOffroadVertices.size());

	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CCatmullRom::RenderRightOffRoad()
{
	glBindVertexArray(m_vaoRightOffroad);

	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffroad.size());

	m_offRoadTexture.Bind();
	glBindVertexArray(m_vaoRightOffroadFace);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_rightOffroadVertices.size());
}

//Rendering

void CCatmullRom::RenderCentreline()
{
	glBindVertexArray(m_vaoCentreline);

	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());

	glPointSize((GLfloat)10.f);
	glDrawArrays(GL_POINTS, 0, m_centrelinePoints.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());

	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());

}

void CCatmullRom::RenderRightRails()
{
	/*glBindVertexArray(m_vaoRightRail);
	glLineWidth((GLfloat)3.f);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightRailPoints.size());*/

	m_railTexture.Bind();
	glBindVertexArray(m_vaoRightRailFace);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_rightRailTriangles.size());


}

void CCatmullRom::RenderLeftRails()
{
	glDisable(GL_CULL_FACE);

	m_railTexture.Bind();
	glBindVertexArray(m_vaoLeftRailFace);
	glDrawArrays(GL_TRIANGLES, 0, m_leftRailTriangles.size());
	glEnable(GL_CULL_FACE);
}

void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	//glDisable(GL_CULL_FACE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	//For some reason it doesn't render the last triangle
	//glDrawArrays(GL_TRIANGLES, m_triangleVertices.size() - 3, 3);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_triangleVertices.size());
	//glEnable(GL_CULL_FACE);
}

void CCatmullRom::Release()
{
	m_texture.Release();
	m_railTexture.Release();

	glDeleteVertexArrays(1, &m_vaoCentreline);
	glDeleteVertexArrays(1, &m_vaoLeftOffsetCurve);
	glDeleteVertexArrays(1, &m_vaoRightOffsetCurve);
	glDeleteVertexArrays(1, &m_vaoTrack);
	glDeleteVertexArrays(1, &m_vaoLeftRail);
	glDeleteVertexArrays(1, &m_vaoLeftRailFace);
	glDeleteVertexArrays(1, &m_vaoRightRail);
	glDeleteVertexArrays(1, &m_vaoRightRailFace);

	//Delete VBOs
	m_vboCentreline.Release();
	m_vboLeftOffsetCurve.Release();
	m_vboRightOffsetCurve.Release();
	m_vboTrack.Release();
	m_vboLeftRail.Release();
	m_vboRightRail.Release();
	m_vboLeftRailFace.Release();
	m_vboRightRailFace.Release();
}

glm::vec3 CCatmullRom::RandomPoint()
{

	return m_centrelinePoints[rand() % m_centrelinePoints.size()];
}

glm::vec3 CCatmullRom::_dummy_vector(0.0f, 0.0f, 0.0f);