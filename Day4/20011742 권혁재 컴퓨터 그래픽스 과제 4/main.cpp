#define _CRT_SECURE_NO_WARNINGS
#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyObject.h"
#include <vec.h>
#include <mat.h>

MyCube cube;
MyObject object;
GLuint program;
GLuint prog_phong;

GLuint uMat;
GLuint uColor;
mat4 g_Mat = mat4(1.0f);
mat4 ModelMat;

int wWidth = 500;
int wHeight = 500;
int RotateA = 0;

float g_Time = 0;
float speed = 0;

float Shiness = 50;
vec4 KSpc = vec4(0.4, 0.4, 0.4, 1);

std::string Objects;

mat4 myLookAt(vec3 e, vec3 a, vec3 u)
{
	mat4 V(1.0f);
	vec3 n = a - e;
	n = n / length(n);

	float d = dot(u, n);
	vec3 v = u - d * n;
	v = v / length(v);

	vec3 w = cross(n, v);

	mat4 Rw(1.0f);

	Rw[0][0] = w.x;	Rw[0][1] = v.x; Rw[0][2] = -n.x;
	Rw[1][0] = w.y;	Rw[1][1] = v.y; Rw[1][2] = -n.y;
	Rw[2][0] = w.z;	Rw[2][1] = v.z; Rw[2][2] = -n.z;

	mat4 Rc(1.0f);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Rc[i][j] = Rw[j][i];
		}
	}

	mat4 Tc = Translate(-e.x, -e.y, -e.z);
	V = Rc * Tc;

	return V;
}

mat4 myOrtho(float l, float r, float b, float t, float zNear, float zFar)
{
	mat4 V(1.0f);

	V[0][0] = 2 / (r - l);
	V[1][1] = 2 / (t - b);
	V[2][2] = 2 / (zFar - zNear);
	V[0][3] = -(r + l) / (r - l);
	V[1][3] = -(t + b) / (t - b);
	V[2][3] = (zNear) / (zFar - zNear);

	return V;
}

mat4 myPerspective(float f, float ratio, float zNear, float zFar)
{
	mat4 P(1.0f);

	float rad = f * 3.141592 / 180.0;

	float sz = 1 / zFar;
	float h = zFar * tan(rad / 2);

	float sy = 1 / h;
	float w = h * ratio;
	float sx = 1 / w;

	mat4 S = Scale(sx, sy, sz);
	mat4 M(1.0f);

	float c = -zNear / zFar;

	M[2][2] = 1 / (c + 1);
	M[2][3] = -c / (c + 1);
	M[3][2] = -1;
	M[3][3] = 0;

	P = M * S;

	return P;
}

void myInit()
{
	cube.Init();
	object.Init(Objects);

	program = InitShader("vshader.glsl", "fshader.glsl");
	prog_phong = InitShader("vphong.glsl", "fphong.glsl");

	float scailng = std::max(std::max(object.sizeXYZ.x, object.sizeXYZ.y), object.sizeXYZ.z);
	ModelMat = Scale(2 / scailng, 2 / scailng, 2 / scailng) * Translate(-object.center);
}

mat4 x_a = Translate(1, 0, 0) * Scale(2, 0.02, 0.02);
mat4 y_a = Translate(0, 1, 0) * Scale(0.02, 2, 0.02);
mat4 z_a = Translate(0, 0, 1) * Scale(0.02, 0.02, 2);

void DrawAxis()
{
	glUseProgram(program);
	switch (RotateA)
	{
	case 0:
		x_a = RotateY(speed) * x_a;
		y_a = RotateY(speed) * y_a;
		z_a = RotateY(speed) * z_a;
		break;
	case 1:
		x_a = RotateX(speed) * x_a;
		y_a = RotateX(speed) * y_a;
		z_a = RotateX(speed) * z_a;
		break;
	case 2:
		x_a = RotateZ(speed) * x_a;
		y_a = RotateZ(speed) * y_a;
		z_a = RotateZ(speed) * z_a;
		break;
	default:
		break;
	}
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * x_a);
	glUniform4f(uColor, 1, 0, 0, 1);
	cube.Draw(program);

	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * y_a);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * z_a);
	glUniform4f(uColor, 0, 0, 1, 1);
	cube.Draw(program);
}

void display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	mat4 ViewMat = myLookAt(vec3(0, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));

	float aspect = wWidth / (float)wHeight;
	float h = 1;

	mat4 ProjMat = myPerspective(40, aspect, 0.1, 100);

	g_Mat = ProjMat * ViewMat;

	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawAxis();

	glUseProgram(prog_phong);
	GLuint uProjMat = glGetUniformLocation(prog_phong, "uProjMat");
	GLuint uModelMat = glGetUniformLocation(prog_phong, "uModelMat");

	switch (RotateA)
	{
	case 0:
		ModelMat = RotateY(speed) * ModelMat;
		break;
	case 1:
		ModelMat = RotateX(speed) * ModelMat;
		break;
	case 2:
		ModelMat = RotateZ(speed) * ModelMat;
		break;
	default:
		break;
	}
	glUniformMatrix4fv(uProjMat, 1, GL_TRUE, ProjMat);
	glUniformMatrix4fv(uModelMat, 1, GL_TRUE, ViewMat * ModelMat);

	vec4 LPos = vec4(0, 0, 2, 1);
	vec4 LCol = vec4(0.9, 0.8, 1.0, 1);
	vec4 KAmb = vec4(0.2, 0.1, 0.2, 1);
	vec4 KDif = vec4(0.8, 0.6, 1.0, 1);

	GLuint uLPos = glGetUniformLocation(prog_phong, "uLPos");
	GLuint uLCol = glGetUniformLocation(prog_phong, "uLCol");
	GLuint uKAmb = glGetUniformLocation(prog_phong, "uKAmb");
	GLuint uKDif = glGetUniformLocation(prog_phong, "uKDif");
	GLuint uKSpc = glGetUniformLocation(prog_phong, "uKSpc");
	GLuint uShine = glGetUniformLocation(prog_phong, "uShine");

	glUniform4f(uLPos, LPos[0], LPos[1], LPos[2], LPos[3]);
	glUniform4f(uLCol, LCol[0], LCol[1], LCol[2], LCol[3]);
	glUniform4f(uKAmb, KAmb[0], KAmb[1], KAmb[2], KAmb[3]);
	glUniform4f(uKDif, KDif[0], KDif[1], KDif[2], KDif[3]);
	glUniform4f(uKSpc, KSpc[0], KSpc[1], KSpc[2], KSpc[3]);
	glUniform1f(uShine, Shiness);

	object.Draw(prog_phong);

	glutSwapBuffers();
}


void idle()
{
	g_Time = g_Time + 0.016;
	Sleep(16);
	glutPostRedisplay();
}

void reshape(int w, int h)
{
	wWidth = w;
	wHeight = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

void myKeyboard(unsigned char c, int x, int y)
{
	if (c == ' ')
		if (speed == 0)
			speed = 1;
		else
			speed = 0;

	if (c == '1') {
		object.PhongShading();
		std::cout << "Using Vertex Normal!" << std::endl;
	}
	if (c == '2') {
		object.FlatShading();
		std::cout << "Using Surface Normal!" << std::endl;
	}
	if (c == '3') {
		if (KSpc.x < 0.5)
			KSpc = KSpc + vec3(0.05, 0.05, 0.05);
		std::cout << "Increasing Specular Effect!" << std::endl;
	}
	if (c == '4') {
		if (KSpc.x > 0.05)
			KSpc = KSpc - vec3(0.05, 0.05, 0.05);
		std::cout << "Decreasing Specular Effect!" << std::endl;
	}
	if (c == '5') {
		if (Shiness < 100)
			Shiness += 10;
		std::cout << "Increasing Shininess!" << std::endl;
	}
	if (c == '6') {
		if (Shiness > 10)
			Shiness -= 10;
		std::cout << "Decreasing Shininess!" << std::endl;
	}
	if (c == 'Q' || c == 'q')
		exit(0);

}

void processMouse(int button, int state, int x, int y) {
	if (state == GLUT_DOWN) {
		if (button == GLUT_RIGHT_BUTTON) {
			RotateA = 2;
		}
		if (button == GLUT_LEFT_BUTTON) {
			RotateA = 1;
		}
		if (button == GLUT_MIDDLE_BUTTON) {
			RotateA = 0;
		}
	}
}

int main(int argc, char** argv)
{
	printf("Input File Path: ");
	std::cin >> Objects;

	while (object.OpenObject(Objects) != 0) {
		printf("Input File Path: ");
		std::cin >> Objects;
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("OpenGL");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMouseFunc(processMouse);
	glutKeyboardFunc(myKeyboard);

	glutMainLoop();

	return 0;
}