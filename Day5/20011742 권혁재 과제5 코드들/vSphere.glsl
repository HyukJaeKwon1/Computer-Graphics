#version 330

in  vec4 vPosition;
in  vec3 vNormal; 
in  vec2 vTexCoord; 

out vec4 pos;
out vec2 texCoord;

out vec3 N3; 
out vec3 L3; 
out vec3 V3;  

uniform mat4 uMat;
uniform mat4 uModelMat; 
uniform mat4 uViewMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uEPos;

void main()
{
	gl_Position  = uMat*vPosition;
	gl_Position *= vec4(1,1,-1,1);
   
	vec4 N = uViewMat*uModelMat*uMat*vec4(vNormal,0); 
	vec4 L = uLPos - uViewMat*uModelMat*vPosition; 
	vec4 V = vec4(0, 0, 0, 1) - uViewMat*uModelMat*vPosition; 

	N3 = normalize(N.xyz); 
	L3 = normalize(L.xyz); 
	V3 = normalize(V.xyz); 

	texCoord = vTexCoord; 
	pos = vPosition;
}
