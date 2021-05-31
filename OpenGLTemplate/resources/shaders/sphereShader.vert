#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;


struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

uniform LightInfo light1; 
uniform LightInfo light2;
uniform LightInfo light3;
uniform MaterialInfo material1; 

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader for Gouraud shading
out vec3 vColour;


vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light.position-p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.f, 90.f));
	vec3 ambient = light.La * material1.Ma;
	if(angle<cutoff)
	{
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v+s);
		float sDotN = max(dot(s,n),0.f);
		vec3 diffuse = light.Ld*material1.Md*sDotN;
		vec3 specular = vec3(0.f);
		if(sDotN >0.f)
		{
			specular = light.Ls*material1.Ms * pow(max(dot(h,n),0.f),material1.shininess);
		}
		return ambient+spotFactor*(diffuse+specular);

	}else

	return ambient;
}

out vec2 vTexCoord;

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, pp. 67 - 68, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light1.position - p));
	vec3 v = normalize(-p.xyz);
	vec3 r = reflect(-s, n);
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0);
	if (sDotN > 0.0)
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0), material1.shininess);
	
	return ambient + diffuse + specular;

}

out vec3 n;
out vec4 p;


void main()
{	

	//vec4 newPosition = Rotate(sin(t), vec4(inPosition, 1.0));
	
	// Transform the vertex spatial position using the projection and modelview matrices
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0);
	
	// Get the vertex normal and vertex position in eye coordinates
	n = normalize(matrices.normalMatrix * inNormal);
	p = matrices.modelViewMatrix * vec4(inPosition, 1.0f);

	// Apply the Phong model to get the colour at this vertex. 
	vColour = PhongModel(p, n)+BlinnPhongSpotlightModel(light2, p, normalize(n)) + BlinnPhongSpotlightModel(light3, p, normalize(n));

	vTexCoord = inCoord;
} 

