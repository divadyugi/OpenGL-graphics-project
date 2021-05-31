#version 400 core

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

//Structure for holding light information
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

//Structures for hold material info
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

//Lights and materials
uniform LightInfo light1;
uniform MaterialInfo material1;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

//The colour output to the fragment shader
out vec3 vColour;
out vec2 vTexCoord;

vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm)
{
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}

//Rotation for animating the coin
uniform float t;

vec4 Rotate(float angle, vec4 position)
{
	mat4 rotationMat;
	rotationMat[0] = vec4(position.x*cos(angle),	0 ,		position.z*sin(angle),		0);
	rotationMat[1] = vec4(0,		    position.y*1,		0,				0);
	rotationMat[2] = vec4(position.x*-sin(angle),	0,	position.z*cos(angle),			0);
	rotationMat[3] = vec4(0,			0,		0,				1);
	vec4 rotationVertex;
	rotationVertex[0] = rotationMat[0][0]+rotationMat[0][2];
	rotationVertex[1] = rotationMat[1][1];
	rotationVertex[2] = rotationMat[2][0]+rotationMat[2][2];
	rotationVertex[3] = 1;
	return rotationVertex;
}

void main()
{
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * Rotate(t, vec4(inPosition, 1.0f));

	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.f);

	vColour = PhongModel(vEyePosition, vEyeNorm);

	vTexCoord = inCoord;
}
