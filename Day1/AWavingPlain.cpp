#include <vgl.h>
#include <InitShader.h>
#include <vec.h>
#include"MyPlain.h"

#define Spacebar 32

int Grid = 30;
int Wave = 0;

GLuint program;
MyPlain plain;

void myInit() {

	plain.init(Grid);

	program = InitShader("vshader.glsl", "fshader.glsl");
}

float myTime = 0;

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(program);

	GLuint uTime = glGetUniformLocation(program, "uTime");
	glUniform1f(uTime, myTime);

	plain.draw(myTime, Wave);

	glFlush();
}

bool bPlay = false;

void idle()
{
	if (bPlay)
		myTime = myTime + 0.0333f;
	Sleep(33);
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		if (Grid > 2) {
			Grid = Grid - 1;
			plain.Redraw(Grid);
		}
		printf("\nDivision: %d, Num.of Triangles: %d, Num. of Vertices: %d", Grid, Grid * Grid * 2, Grid * Grid * 6);
		break;
	case '2':
		Grid = Grid + 1;
		plain.Redraw(Grid);
		printf("\nDivision: %d, Num.of Triangles: %d, Num. of Vertices: %d", Grid, Grid * Grid * 2, Grid * Grid * 6);
		break;
	case Spacebar:
		if (bPlay == 0)
			bPlay = 1;
		else
			bPlay = 0;
		break;
	case 'w':case'W':
		if (Wave == 0)
			Wave = 1;
		else
			Wave = 0;
		break;
	case 'q':case 'Q':
		exit(1);
		break;
	default:
		break;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow("Waving Plain");

	printf("A Waving Color Plain\nProgramming Assignment #1 for Computer Graphics\n");
	printf("Department of Software, Sejong University\n\n");
	printf("--------------------------------------------------------------\n");
	printf("'1' key: Decreasing the Number of Division\n");
	printf("'2' key: Increasing the Number of Division\n");
	printf("'w' key: Showing/hiding the waving pattern\n");
	printf("Spacebar: starting/stoping rotating and waving\n");
	printf("\n'Q' Key: Exit the program.\n");
	printf("--------------------------------------------------------------\n");
	printf("\nDivision: %d, Num.of Triangles: %d, Num. of Vertices: %d", Grid, Grid * Grid * 2, Grid * Grid * 6);

	glewExperimental = true;
	glewInit();

	myInit();

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}