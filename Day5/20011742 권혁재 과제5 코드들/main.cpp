#define _CRT_SECURE_NO_WARNINGS
#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MySphere.h"
#include "MyObject.h"
#include "Targa.h"

#include <vec.h>
#include <mat.h>

MyCube cube;
MySphere sphere;
MyObject object;

GLuint program;
GLuint phong_prog;
GLuint sphere_prog;
GLuint diffuse_prog;

mat4 g_Mat = mat4(1.0f);

char tga_name[256];
char file_name[256];
FILE* FileName = NULL;

float g_time = 0;
float g_aspect = 1;

bool isRotate = false;
int DrawObject = 0;

float r = 6.0;
float theta1 = 1.5*3.141592;
float theta2 = 0;
float eye_x = cos(theta1);
float eye_y = theta2;
float eye_z = sin(theta1);
vec3 pos = normalize(vec3(eye_x, eye_y, eye_z))*-r;

vec2 p1, p2;

float Fresnel = 10.0;
bool shader = false;
bool m_button;

mat4 myLookAt(vec3 e, vec3 a, vec3 u)
{
	mat4 V = mat4(1.0f);
	
	u = normalize(u);
	vec3 n = normalize(a - e);
	float d = dot(u, n);
	vec3 v = normalize(u-d*n);
	vec3 w = cross(n, v);

	V[0] = vec4(w, dot(-w, e));
	V[1] = vec4(v, dot(-v, e));
	V[2] = vec4(-n, dot(n, e));
	
	return V;
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	vec3 c = vec3((l+r)/2, (b+t)/2, -(zNear)/2);
	mat4 T = Translate(-c);
	mat4 S = Scale(2/(r-l), 2/(t-b), -1/(-zNear+zFar));
	mat4 V = S*T;

	return V;
}

mat4 myPerspective(float angle, float aspect, float zNear, float zFar)
{
	float rad = angle*3.141592/180.0f;
	mat4 V(1.0f);
	float h = 2*zFar*tan(rad/2);
	float w = aspect*h;
	mat4 S = Scale(2/w, 2/h, 1/zFar);

	float c = -zNear/zFar;
	
	mat4 Mpt(1.0f);
	Mpt[2] = vec4(0, 0, 1/(c+1), -c/(c+1));
	Mpt[3] = vec4(0, 0, -1, 0);
	
	V = Mpt*S;

	return V;
}

void InitTexture()
{
	GLuint myTex[2];
	glGenTextures(2, myTex);
	for (int i=0; i<2; i++)
	{
		STGA image;
		if (i==0)
		{
			image.loadTGA(tga_name);
			glActiveTexture(GL_TEXTURE0);
		}
		else if (i==1)
		{
			image.loadTGA(file_name);
			glActiveTexture(GL_TEXTURE1);
		}
		glBindTexture(GL_TEXTURE_2D, myTex[i]);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
		image.destroy();

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}	
}

void myInit()
{
	cube.Init();
	sphere.Init(40,40);
	object.Init("bunny.obj");

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
	
	phong_prog = InitShader("vPhong.glsl", "fPhong.glsl");
	glUseProgram(phong_prog);

	sphere_prog = InitShader("vSphere.glsl", "fSphere.glsl");
	glUseProgram(sphere_prog);
	
	diffuse_prog = InitShader("vDiffuse.glsl", "fDiffuse.glsl");
	glUseProgram(diffuse_prog);

	InitTexture();
		
}

void DrawAxis()
{
	glUseProgram(program);
	GLuint uMat = glGetUniformLocation(program, "uMat");
	GLuint uColor = glGetUniformLocation(program, "uColor");

	mat4 x_a= Translate(0.5,0,0)*Scale(1,0.05,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	mat4 y_a= Translate(0,0.5,0)*Scale(0.05,1,0.05);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	mat4 z_a= Translate(0,0,0.5)*Scale(0.05,0.05,1);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat*z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void display()
{
	glClearColor(0,0,0,1);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	vec3 ePos = pos;
	mat4 ModelMat = RotateY(g_time);
	mat4 ViewMat = myLookAt(ePos, vec3(0,0,0), vec3(0,1,0));
	mat4 ProjMat = myPerspective(45, g_aspect, 0.01, 100.0f);

		
	g_Mat = ProjMat*ViewMat*ModelMat;

	vec4 lPos = vec4(2, 2, 0, 1);			 
	vec4 lAmb = vec4(0.5, 0.5, 0.5, 1);		
	vec4 lDif = vec4(1, 1, 1, 1);
	vec4 lSpc = lDif; 

	vec4 mAmb = vec4(0.3, 0.3, 0.3, 1);		
	vec4 mDif = vec4(0.8, 0.8, 0.8, 1);		
	vec4 mSpc = vec4(0.3, 0.3, 0.3, 1); 
	float mShiny = 50;

	vec4 amb = lAmb*mAmb;					
	vec4 dif = lDif*mDif;					
	vec4 spc = lSpc*mSpc; 

	glUseProgram(sphere_prog);
	
	sphere.Draw(sphere_prog);
	GLuint uMat = glGetUniformLocation(sphere_prog, "uMat");
	GLuint uTexture = glGetUniformLocation(sphere_prog, "uTexture");

	glUniformMatrix4fv(uMat, 1, true, ProjMat*ViewMat*Scale(10,10,10));
	GLuint uModelMat = glGetUniformLocation(sphere_prog, "uModelMat");
	GLuint uViewMat = glGetUniformLocation(sphere_prog, "uViewMat");
	GLuint uLPos = glGetUniformLocation(sphere_prog, "uLPos"); 
	GLuint uEPos = glGetUniformLocation(sphere_prog, "uEPos"); 
	
	glUniformMatrix4fv(uModelMat, 1, true, ModelMat); 
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat); 
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]); 
	glUniform4f(uEPos, ePos[0], ePos[1], ePos[2], 1);
	glUniform1i(uTexture, 0);
	
	glUseProgram(phong_prog);

	uModelMat = glGetUniformLocation(phong_prog, "uModelMat");
	uViewMat = glGetUniformLocation(phong_prog, "uViewMat");
	GLuint uProjMat = glGetUniformLocation(phong_prog, "uProjMat"); 
	uLPos = glGetUniformLocation(phong_prog, "uLPos"); 
	GLuint uAmb = glGetUniformLocation(phong_prog, "uAmb"); 
	GLuint uDif = glGetUniformLocation(phong_prog, "uDif"); 
	GLuint uSpc = glGetUniformLocation(phong_prog, "uSpc"); 
	GLuint uShininess = glGetUniformLocation(phong_prog, "uShininess"); 
	uEPos = glGetUniformLocation(phong_prog, "uEPos"); 
	uTexture = glGetUniformLocation(phong_prog, "uTexture");
	GLuint uDiffuse = glGetUniformLocation(phong_prog, "uDiffuse");
	GLuint uFresnel = glGetUniformLocation(phong_prog, "uFresnel");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat); 
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat); 
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat); 
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]); 
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]); 
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]); 
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]); 
	glUniform1f(uShininess, mShiny); 
	glUniform4f(uEPos, ePos[0], ePos[1], ePos[2], 1);
	glUniform1i(uTexture, 0);
	glUniform1i(uDiffuse, 1);
	glUniform1f(uFresnel, Fresnel);

	if (shader == false)
	{
		if (DrawObject == 1)
			sphere.Draw(phong_prog);
		else if (DrawObject == 2)
			object.Draw(phong_prog);
	}

	glUseProgram(diffuse_prog);

	uModelMat = glGetUniformLocation(diffuse_prog, "uModelMat");
	uViewMat = glGetUniformLocation(diffuse_prog, "uViewMat");
	uProjMat = glGetUniformLocation(diffuse_prog, "uProjMat"); 
	uLPos = glGetUniformLocation(diffuse_prog, "uLPos"); 
	uAmb = glGetUniformLocation(diffuse_prog, "uAmb"); 
	uDif = glGetUniformLocation(diffuse_prog, "uDif"); 
	uSpc = glGetUniformLocation(diffuse_prog, "uSpc"); 
	uShininess = glGetUniformLocation(diffuse_prog, "uShininess"); 
	uEPos = glGetUniformLocation(diffuse_prog, "uEPos"); 
	uTexture = glGetUniformLocation(diffuse_prog, "uTexture");
	uDiffuse = glGetUniformLocation(diffuse_prog, "uDiffuse");
	uFresnel = glGetUniformLocation(diffuse_prog, "uFresnel");

	glUniformMatrix4fv(uModelMat, 1, true, ModelMat); 
	glUniformMatrix4fv(uViewMat, 1, true, ViewMat); 
	glUniformMatrix4fv(uProjMat, 1, true, ProjMat); 
	glUniform4f(uLPos, lPos[0], lPos[1], lPos[2], lPos[3]); 
	glUniform4f(uAmb, amb[0], amb[1], amb[2], amb[3]); 
	glUniform4f(uDif, dif[0], dif[1], dif[2], dif[3]); 
	glUniform4f(uSpc, spc[0], spc[1], spc[2], spc[3]); 
	glUniform1f(uShininess, mShiny); 
	glUniform4f(uEPos, ePos[0], ePos[1], ePos[2], 1);
	glUniform1i(uTexture, 0);
	glUniform1i(uDiffuse, 1);
	glUniform1f(uFresnel, Fresnel);

	if (shader == true)
	{
		if (DrawObject == 1)
			sphere.Draw(diffuse_prog);
		else if (DrawObject == 2)
			object.Draw(diffuse_prog);
	}

	Sleep(16);
	glutSwapBuffers();
}

void idle()
{
	if (isRotate == true)
		g_time += 1;

	glutPostRedisplay();
}

void keyboard(unsigned char ch, int x, int y)
{
	switch (ch)
	{
		case 'q':
		case 'Q':
			DrawObject += 1;
			if (DrawObject == 3)
				DrawObject = 0;
			break;
		case '1': 
			Fresnel -= 0.5;
			if (Fresnel < 0.5)
				Fresnel = 0.5;			
			printf("Fresnel Power = %.6f\n", Fresnel);
			break;
		case '2':
			Fresnel += 0.5;
			if (Fresnel > 11.0)
				Fresnel = 11.0;
			printf("Fresnel Power = %.6f\n", Fresnel);
			break;
		case '3':
			if (!shader)
			{
				shader = true;
				printf("Diffuse LightMap On\n");
			}
			else
			{
				shader = false;
				printf("Diffuse LightMap Off\n");
			}
				break;
		case VK_SPACE:
			if (!isRotate)
				isRotate = true;
			else
				isRotate = false;
			break;
		default:
			break;
	}
}

void MouseClick(int button,int st, int x, int y)
{
	if (button == 0)
	{
		m_button = false;
		if (st == 0)
			p1 = vec2(x,y);
	}
	else if (button == 2)
	{
		m_button = true;
		if (st == 0)
			p1 = vec2(x,y);
	}

	glutPostRedisplay();
}

void MouseDrag(int x, int y)
{
	p2 = vec2(x,y);
	vec2 delta = vec2(p2.x-p1.x, p2.y-p1.y); 
	p1 = p2;
	
	if (!m_button)
	{
		theta1 -= delta.x/500;
		eye_x = cos(theta1);
		eye_z = sin(theta1);

		theta2 += delta.y/500;
		eye_y = theta2;

		pos = vec3(eye_x,eye_y,eye_z);
		pos = normalize(pos)*-r;
	}
	else
	{
		r -= delta.y/125;

		if (r < 3.0) r = 3.0;
		else if (r > 25.0) r = 25.0;

		pos = vec3(eye_x,eye_y,eye_z);
		pos = normalize(pos)*-r;
	}
	
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	
	g_aspect = w/float(h);
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	while(true)
	{
		printf("Input Image Group Name: ");
		scanf("%s", &tga_name);
		strcpy(file_name, tga_name);
		strcat(tga_name, "_spheremap.tga");
		strcat(file_name, "_diffusemap.tga");
		
		FileName = fopen(tga_name, "r");
		if (!FileName)
		{
			printf("File not Found!\n");
			continue;
		}
		else break;
		FileName = fopen(file_name, "r");
		if (!FileName)
		{
			printf("File not Found!\n");
			continue;
		}
		else break;
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(800,500);

	glutCreateWindow("Homework 5");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n",	glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMouseFunc(MouseClick);
	glutMotionFunc(MouseDrag);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}


