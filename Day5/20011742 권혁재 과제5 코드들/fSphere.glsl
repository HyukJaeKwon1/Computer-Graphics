#version 330

in  vec4 pos;
in	vec2 texCoord;
in	vec3 N3; 
in	vec3 L3; 
in	vec3 V3;  

out vec4 fColor;

uniform mat4 uModelMat; 
uniform mat4 uViewMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uEPos;
uniform sampler2D uTexture;

void main()
{
	vec3 N = normalize(N3); 
	vec3 V = normalize(V3); 

    float NV = dot(N, V);

	vec2 tex = texCoord;
	tex.x = 1.0-tex.x;
	if (NV < 0.0) fColor = texture(uTexture, tex);
	else discard;
}
