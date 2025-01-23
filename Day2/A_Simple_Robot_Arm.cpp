#include <vgl.h>
#include <InitShader.h>
#include <mat.h>
#include "MyCube.h"
#include "MyPyramid.h"
#include "MyTarget.h"

MyCube cube;
MyPyramid pyramid;
MyTarget target(&cube);

GLuint program;
GLuint uMat;

mat4 CTM;

vec4 d;

bool bPlay = false;
bool bChasingTarget = false;
bool bDrawTarget = false;

float ang1 = 0;
float ang2 = 0;
float ang3 = 0;

int flag1 = 1;
int flag2 = 1;
int flag3 = 1;

float dtc;
float check;

void myInit()
{
	cube.Init();
	pyramid.Init();

	program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);
}

float g_time = 0;

void calcEnd(float ang1, float ang2, float ang3) {
	mat4 temp = CTM;
	d = vec4(0.5, 0, 0, 1);
	CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
	CTM = CTM * RotateZ(ang1);
	CTM = CTM * Translate(0, 0.4, 0) * RotateZ(ang2);
	CTM = CTM * Translate(0, 0.4, 0) * RotateZ(ang3);
	d = CTM * Scale(0.4, 0, 0) * d;
	CTM = temp;
}

void drawRobotArm(float ang1, float ang2, float ang3)
{
	mat4 temp = CTM;

	// BASE
	mat4 M(1.0);

	CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
	M = Translate(0, 0, 0.08) * Scale(0.3, 0.3, 0.06);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// BASE 2
	M = Translate(0, 0, -0.08) * Scale(0.3, 0.3, 0.06);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	pyramid.Draw(program);

	// BASE 연결부
	M = Scale(0.05, 0.05, 0.25);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Upper Arm
	CTM *= RotateZ(ang1);
	M = Translate(0, 0.2, 0) * Scale(0.1, 0.5, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang2);
	M = Translate(0, 0.2, -0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm 2
	M = Translate(0, 0.2, 0.075) * Scale(0.1, 0.5, 0.05);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Lower Arm 연결부
	M = Scale(0.05, 0.05, 0.25);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Hand
	CTM *= Translate(0, 0.4, 0) * RotateZ(ang3);
	M = Scale(0.4, 0.15, 0.1);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	// Hand 연결부
	M = Scale(0.05, 0.05, 0.25);
	glUniformMatrix4fv(uMat, 1, true, CTM * M);
	cube.Draw(program);

	calcEnd(ang1, ang2, ang3);

	CTM = temp;
}

float distance(vec3 p)
{
	calcEnd(ang1, ang2, ang3);
	if ((int)(g_time * 30) % 360 >= 90.0 && (int)(g_time * 30) % 360 <= 270.0)
	{
		return ((d.x + p.x) * (d.x + p.x)) + ((d.y + 0.4 - p.y) * (d.y + 0.4 - p.y)) + ((d.z - p.z) * (d.z - p.z));
	}
	else
	{
		return ((d.x - p.x) * (d.x - p.x)) + ((d.y + 0.4 - p.y) * (d.y + 0.4 - p.y)) + ((d.z - p.z) * (d.z - p.z));
	}
}

void computeAngle()
{
	vec3 targetPosition = target.GetPosition(g_time);

	dtc = distance(targetPosition);

	for (int i = 0; i < 10; i++)
	{
		if (dtc < 0.01)
			break;

		if (flag1 == 1)
			ang1 = ang1 + 0.6;
		else
			ang1 = ang1 - 0.6;

		check = dtc;
		dtc = distance(targetPosition);

		if (dtc - check > 0)
		{
			if (flag1 == 1)
			{
				flag1 = 0;
				ang1 = ang1 - 1.2;
				check = dtc;
				dtc = distance(targetPosition);
			}
			else
			{
				flag1 = 1;
				ang1 = ang1 + 1.2;
				check = dtc;
				dtc = distance(targetPosition);
			}
		}
		if (dtc < 0.01)
			break;

		if (flag2 == 1)
			ang2 = ang2 + 0.3;
		else
			ang2 = ang2 - 0.3;

		check = dtc;
		dtc = distance(targetPosition);

		if (dtc - check > 0)
		{
			if (flag2 == 1)
			{
				flag2 = 0;
				ang2 = ang2 - 0.6;
				check = dtc;
				dtc = distance(targetPosition);
			}
			else
			{
				flag2 = 1;
				ang2 = ang2 + 0.6;
				check = dtc;
				dtc = distance(targetPosition);
			}
		}

		if (dtc < 0.01)
			break;

		if (flag3 == 1)
			ang3 = ang3 + 0.15;
		else
			ang3 = ang3 - 0.15;

		check = dtc;
		dtc = distance(targetPosition);
		if (dtc - check > 0)
		{
			if (flag3 == 1)
			{
				flag3 = 0;
				ang3 = ang3 - 0.3;
				check = dtc;
				dtc = distance(targetPosition);
			}
			else
			{
				flag3 = 1;
				ang3 = ang3 + 0.3;
				check = dtc;
				dtc = distance(targetPosition);
			}
		}

	}
}

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GLuint uColor = glGetUniformLocation(program, "uColor");
	glUniform4f(uColor, -1, -1, -1, -1);


	uMat = glGetUniformLocation(program, "uMat");
	CTM = Translate(0, -0.4, 0) * RotateY(g_time * 30);
	drawRobotArm(ang1, ang2, ang3);


	glUniform4f(uColor, 1, 0, 0, 1);
	if (bDrawTarget == true)
		target.Draw(program, CTM, g_time);

	glutSwapBuffers();
}

void myIdle()
{
	if (bPlay)
	{
		g_time += 1 / 60.0f;
		Sleep(1 / 60.0f * 1000);

		if (bChasingTarget == false)
		{
			ang1 = 45 * sin(g_time * 3.141592);
			ang2 = 60 * sin(g_time * 2 * 3.141592);
			ang3 = 30 * sin(g_time * 3.141592);
		}
		else
			computeAngle();

		glutPostRedisplay();
	}
}

void myKeyboard(unsigned char c, int x, int y)
{

	switch (c)
	{
	case '1':
		bChasingTarget = !bChasingTarget;
		break;
	case '2':
		bDrawTarget = !bDrawTarget;
		break;
	case '3':
		target.toggleRandom();
		break;
	case ' ':
		bPlay = !bPlay;
		break;
	default:
		break;
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Simple Robot Arm");

	glewExperimental = true;
	glewInit();

	myInit();
	glutDisplayFunc(myDisplay);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);

	glutMainLoop();

	return 0;
}