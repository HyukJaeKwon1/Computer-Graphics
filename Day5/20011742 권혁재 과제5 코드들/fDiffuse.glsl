#version 330

in vec3 N3; 
in vec3 L3; 
in vec3 V3; 

in vec4 cV;
in vec4 cN;
in vec4 cT;

out vec4 fColor;

uniform mat4 uModelMat;
uniform mat4 uViewMat;  
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uEPos;
uniform vec4 uAmb; 
uniform vec4 uDif; 
uniform vec4 uSpc; 
uniform float uShininess; 
uniform sampler2D uTexture;
uniform sampler2D uDiffuse;
uniform float uFresnel;

void main()
{
	vec3 N = normalize(N3); 
	vec3 L = normalize(L3); 
	vec3 V = normalize(V3); 
	vec3 H = normalize(V+L); 

    float NL = max(dot(N, L), 0); 
	float VR = pow(max(dot(H, N), 0), uShininess); 
	
	vec4 phong_color = uAmb + uDif*NL;

	vec3 wV = normalize(cV.xyz);
	vec3 wN = normalize(cN.xyz);
	vec3 wT = normalize(cT.xyz);

	vec3 dir1 = reflect(wV, wN);
	vec3 dir2 = reflect(wT, wN);
	dir1.y = -dir1.y;
	dir2.y = -dir2.y;
	
	float u = atan(dir1.x/dir1.z)/3.141592;
	float v = acos(dir1.y)/3.141592;

	if (dir1.z < 0)
		u = u/2+0.25;
	else
		u = u/2+0.75;

	u = 1.0-u;
	vec4 cRfl = texture(uTexture, vec2(u,v));

	u = atan(dir2.x/dir2.z)/3.141592;
	v = acos(dir2.y)/3.141592;

	if (dir2.z < 0)
		u = u/2-0.25;
	else
		u = u/2-0.75;
	u = 1.0-u;
	v = 1.0-v;
	vec4 dif_color = texture(uDiffuse, vec2(u,v));
	dif_color = dif_color+0.15;

	float FresnelPower = uFresnel;
	float F = 0.1;

	float ratio = F+(1.0-F)*pow((1.0+dot(wV, wN)), FresnelPower);
	if (FresnelPower>10) ratio = 0; // 10보다 클 경우 반사 없음
	fColor = mix(dif_color, cRfl, ratio);
	fColor.w = 1; 

}
