#version 400 core

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform MaterialInfo material1; 

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec3 vColour;	// Colour computed using reflectance model
out vec2 vTexCoord;	// Texture coordinate

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

uniform vec3 positions[25];
uniform float angles[25];
uniform mat4 translationMatrices[25];

 /*vec4 Rotate(float angle, vec4 position, vec3 transformation)
{
	mat4 rotationMat;
	rotationMat[0] = vec4(position.x*cos(angle),	0 ,		position.z*sin(angle),		position.x+transformation.x);
	rotationMat[1] = vec4(0,		    position.y*1,		0,				position.y+transformation.y);
	rotationMat[2] = vec4(position.x*-sin(angle),	0,	position.z*cos(angle),			position.z+transformation.z);
	rotationMat[3] = vec4(0,			0,		0,				1);
	vec4 rotationVertex;
	rotationVertex[0] = rotationMat[0][0]+rotationMat[0][2]+rotationMat[0][3];
	rotationVertex[1] = rotationMat[1][1]+rotationMat[1][3];
	rotationVertex[2] = rotationMat[2][0]+rotationMat[2][2]+rotationMat[2][3];
	rotationVertex[3] = 1;
	return rotationVertex;
}*/



//Matrix rotation from: https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/10.3.asteroids_instanced/asteroids_instanced.cpp

void main()
{

	
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * translationMatrices[gl_InstanceID]* vec4(inPosition, 1.f);

	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);

	vColour = PhongModel(vEyePosition, vEyeNorm);

	vTexCoord = inCoord;
}