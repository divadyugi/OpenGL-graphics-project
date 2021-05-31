#version 400 core 

in vec3 vColour;			// Note: colour is smoothly interpolated (default)
out vec4 vOutputColour;
in vec2 vTexCoord;

uniform sampler2D sampler0;
uniform bool bUseTexture;


void main()
{
	vec4 vTexColour = texture(sampler0, vTexCoord);	
	
	//vec3 vColour = PhongModel(p, normalize(n));
	//vec3 vColour2 = BlinnPhongSpotlightModel(light2, p, normalize(n));
	if (bUseTexture)
		if(vTexColour.a <0.1)
			discard;
		else
		vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
	else
		vOutputColour = vec4(vColour, 1.0f);
	
}
