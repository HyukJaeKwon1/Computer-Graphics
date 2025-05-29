#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;
out vec3 N3;
out vec3 V3;
out vec3 L3;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uKAmb;
uniform vec4 uKDif;
uniform vec4 uKSpc;
uniform float uShine;

void main()
{
	gl_Position  = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1,1,-1,1);	// z축 방향이 반대임

	vec4 P = uModelMat*vPosition;
	vec4 L = uLPos - P;
	L3 = normalize(L.xyz);
	vec4 N = uModelMat*vec4(vNormal,0);
	N3 = normalize(N.xyz);
	vec4 V = vec4(0,0,0,1) - P;
	V3 = normalize(V.xyz);

	color = vColor;
}