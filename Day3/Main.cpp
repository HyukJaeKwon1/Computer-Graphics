#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include <iostream>
#include <vector>
#include "MyCube.h"
#include "MyUtil.h"

#include <vec.h>
#include <mat.h>

#define MAZE_FILE	"maze.txt"

using namespace std;

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
vec3 goalPos = vec3(0, 0, 0);

int MazeSize;
char maze[255][255] = { 0 };

int dirX[8] = { 0,0,-1,1, -1, 1, -1, 1 };
int dirY[8] = { -1,1,0,0, -1, -1, 1, 1 };

float cameraSpeed = 0.1;

float g_time = 0;

double turn = 0 + ((2 * 3.141592653589) / 4) * 3;

bool LeftTurn = false;
bool RightTurn = false;
bool Navigating = false;

int cnt = 0;
int maxcount = 0;
int navigator = 0;

int turncount = 0;

typedef struct node {
	int h = 0;
	int f = 0;
	int g = 0;
	pair<int, int> parent;
	pair<int, int> index;
	pair<int, int> child;
};

vector<node> Close;
vector<node> Open;
vector<node> Goal;

vector<vec3> navigateArrow;

pair<int, int> goal;
pair<int, int> start;

inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

bool CheckCollision(vec3 _pos) {
	float u = 1;
	float v = 0.15;

	for (int i = 0; i < 8; i++) {

		vec3 pos = _pos;

		vec3 leftTopPosition = vec3(MazeSize / 2.0 + dirX[i] * v, 0, MazeSize / 2.0 + dirY[i] * v);

		pos = pos + leftTopPosition;


		if (maze[(int)(pos.x)][(int)(pos.z)] == '*') {
			maze[(int)(pos.x)][(int)(pos.z)] = '|';
			return true;
		}
	}
	return false;
}

void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
				cameraPos = getPositionFromIndex(i, j);
			if (maze[i][j] == 'G') {			// Setup Goal Position
				goalPos = getPositionFromIndex(i, j);
				goal.first = j;
				goal.second = i;
			}
		}
	}
	fclose(file);
}

float RTD(float angle) {
	return angle / 3.141592 * 180;
}

float DTR(float angle) {
	return angle / 180 * 3.141592;
}

float GetAngle(vec3 v1, vec3 v2) {
	v1 = normalize(v1);
	v2 = normalize(v2);

	int direction = 1;
	if (cross(v1, v2).y > 0)
		direction = -1;

	return direction * RTD(acos(dot(v1, v2)));
}

void DrawMaze(bool check)
{
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++) {


			if (maze[i][j] == '*')
			{
				vec3 color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
			}

			if (maze[i][j] == '|') {
				vec3 color = vec3(255, 0, 0);
				mat4 ModelMat = Translate(getPositionFromIndex(i, j));
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
				if (check)
					maze[i][j] = '*';
			}
		}
}

void myInit()
{
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");
}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
}

void SetAngle(int a) {
	switch (a) {
	case 1:
		turn = 0 + ((2 * 3.141592653589) / 4) * 3;
		break;

	case 2:
		turn = 0 + ((2 * 3.141592653589) / 4) * 1;
		break;

	case 3:
		turn = 0 + ((2 * 3.141592653589) / 4) * 2;
		break;

	case 4:
		turn = 0 + ((2 * 3.141592653589) / 4) * 4;
		break;
	}
}

void DrawGoalRoad() {
	float w = 1;

	for (int i = 0; i < Goal.size(); i++) {
		vec3 pos = getPositionFromIndex(Goal[i].index.second, Goal[i].index.first);

		pair<int, int> parent = Goal[i].parent;
		pair<int, int> cur = Goal[i].index;
		pair<int, int> child = Goal[i].child;
		mat4 m;

		if (Goal.size() - 1 == i) {
			if (cur.first == child.first && cur.second < child.second) {
				m = Translate(pos.x + 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
			}
			else if (cur.first == child.first && cur.second > child.second) {
				m = Translate(pos.x - 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
			}
			else if (cur.second == child.second && cur.first < child.first) {
				m = Translate(pos.x, -0.3, pos.z + 0.25) * Scale(0.1, 0.1, w / 2);
			}
			else {
				m = Translate(pos.x, -0.3, pos.z - 0.25) * Scale(0.1, 0.1, w / 2);
			}


			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
			cube.Draw(program);

			continue;
		}

		if ((parent.first == cur.first && parent.second > cur.second && child.second == cur.second && child.first < cur.first) || (child.first == cur.first && child.second > cur.second && parent.second == cur.second && parent.first < cur.first)) { // ┕ 모양
			m = Translate(pos.x, -0.3, pos.z - 0.25) * Scale(0.1, 0.1, w / 2);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
			cube.Draw(program);

			m = Translate(pos.x + 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
		}
		else if ((parent.first == cur.first && parent.second > cur.second && child.second == cur.second && child.first > cur.first) || (child.first == cur.first && child.second > cur.second && parent.second == cur.second && parent.first > cur.first)) { // ┎ 모양
			m = Translate(pos.x, -0.3, pos.z + 0.25) * Scale(0.1, 0.1, w / 2);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
			cube.Draw(program);

			m = Translate(pos.x + 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
		}
		else if ((parent.first == cur.first && parent.second < cur.second && child.second == cur.second && child.first > cur.first) || (child.first == cur.first && child.second < cur.second && parent.second == cur.second && parent.first > cur.first)) { // ┐ 모양
			m = Translate(pos.x, -0.3, pos.z + 0.25) * Scale(0.1, 0.1, w / 2);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
			cube.Draw(program);

			m = Translate(pos.x - 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
		}
		else if ((parent.first == cur.first && parent.second < cur.second && child.second == cur.second && child.first < cur.first) || (child.first == cur.first && child.second < cur.second && parent.second == cur.second && parent.first < cur.first)) { // ┛ 모양
			m = Translate(pos.x, -0.3, pos.z - 0.25) * Scale(0.1, 0.1, w / 2);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
			cube.Draw(program);

			m = Translate(pos.x - 0.25, -0.3, pos.z) * Scale(w / 2, 0.1, 0.1);
		}
		else if (parent.first != cur.first && parent.second == cur.second) {
			m = Translate(pos.x, -0.3, pos.z) * Scale(0.1, 0.1, w);
		}
		else if (parent.second != cur.second && parent.first == parent.first) {
			m = Translate(pos.x, -0.3, pos.z) * Scale(w, 0.1, 0.1);
		}

		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1.0f, 0.0f, 0.0f, 1.0f);
		cube.Draw(program);
	}
}

void drawCamera()
{
	float cameraSize = 0.5;

	mat4 ModelMat = Translate(cameraPos) * RotateY(GetAngle(viewDirection, vec3(-1, 0, 0))) * Scale(vec3(cameraSize));

	if (LeftTurn) {
		turn = turn - 0.1;
	}

	if (RightTurn) {
		turn = turn + 0.1;
	}


	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * Scale(vec3(cameraSize / 2)) * RotateY(GetAngle(viewDirection, vec3(-1, 0, 0)));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);

	LeftTurn = false;
	RightTurn = false;
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}


void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawMaze(bDrawCamera);
	drawGoal();
	DrawGoalRoad();

	if (bDrawCamera)
		drawCamera();
}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;
	mat4 ViewMat = myLookAt(cameraPos, cameraPos + viewDirection, vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera

	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);


	glutSwapBuffers();
}

pair<int, int> TakeIndex() {
	vec3 pos = vec3(cameraPos.x + MazeSize / 2.0, 0, cameraPos.z + MazeSize / 2.0);

	pair<int, int> index;
	index.second = pos.x;
	index.first = pos.z;

	return index;
}

void FindOpen(pair<int, int> currentIndex) {
	for (int i = 0; i < 4; i++) {
		pair<int, int> tmp;

		tmp.first = currentIndex.first + dirY[i];
		tmp.second = currentIndex.second + dirX[i];

		bool isCloseNode = false;
		for (int i = 0; i < Close.size(); i++) {
			if (tmp == Close[i].index)
				isCloseNode = true;
		}

		if (isCloseNode)
			continue;

		if (maze[tmp.second][tmp.first] == ' ' || maze[tmp.second][tmp.first] == 'G' || maze[tmp.second][tmp.first] == 'C') {
			node child;

			child.parent = currentIndex;

			int sy = abs(start.first - tmp.first);
			int sx = abs(start.second - tmp.second);

			child.g = sx + sy;

			int gy = abs(goal.first - tmp.first);
			int gx = abs(goal.second - tmp.second);

			child.h = gx + gy;

			child.f = child.g + child.h;
			child.index = tmp;

			bool isClear = true;

			for (int j = 0; j < Open.size(); j++) {
				if (Open[j].index == child.index) {
					if (Open[j].g > child.g) {
						Open.erase(Open.begin() + j);
					}
					else {
						isClear = false;
					}
				}
			}

			if (isClear) {
				Open.push_back(child);
			}
		}
	}
}

pair<int, int> SetClose() {
	pair<int, int> closeIndex;

	if (Open.empty())
		return closeIndex;
	node index = Open[0];
	int indexNum = 0;

	for (int i = 1; i < Open.size(); i++) {
		if (index.f > Open[i].f) {
			index = Open[i];
			indexNum = i;
		}
	}

	Open.erase(Open.begin() + indexNum);

	closeIndex = index.index;
	Close.push_back(index);

	return closeIndex;
}

void astar() {
	Open.clear();
	Close.clear();
	Goal.clear();

	start = TakeIndex();
	pair<int, int> currentIndex = start;

	node first;

	first.index = currentIndex;
	Close.push_back(first);

	int f = 0;
	int s = 0;

	while (1) {
		FindOpen(currentIndex);
		currentIndex = SetClose();

		if (currentIndex == goal) {
			break;
		}
		if ((GetAsyncKeyState('O') & 0x8000) == 0x8000) {
			break;
		}

	}

	pair<int, int> parentIndex;
	parentIndex = Close[Close.size() - 1].parent;
	Goal.push_back(Close[Close.size() - 1]);

	for (int i = Close.size() - 1; i >= 0; i--) {

		if (parentIndex == start) {
			Close[i].child = Goal[Goal.size() - 1].index;
		}

		if (parentIndex == Close[i].index) {
			Goal.push_back(Close[i]);
			parentIndex = Close[i].parent;
		}


	}

	for (int i = 1; i < Goal.size() - 1; i++) {
		Goal[i].child = Goal[i - 1].index;
	}
}

void Navigate() {
	if (Goal.empty())
		return;

	navigateArrow.clear();
	Navigating = true;

	int angle = 0;

	for (int i = Goal.size() - 1; i > 0; i--) {
		vec3 start = getPositionFromIndex(Goal[i].index.second, Goal[i].index.first);
		vec3 end = getPositionFromIndex(Goal[i - 1].index.second, Goal[i - 1].index.first);

		vec3 arrow = normalize(end - start);
		navigateArrow.push_back(arrow);
	}
}

int SetNaviArrow(vec3 arrow) {
	if (arrow.z < 0)
		return 1;

	if (arrow.z > 0)
		return 2;

	if (arrow.x < 0)
		return 3;

	if (arrow.x > 0) {
		return 4;
	}
}

void CheckTurnDirection(int a, int b) {
	double angle = (2 * 3.141592653589 / 4) * 0.1;

	if (a == 1) {
		if (b == 3) {
			turn -= angle;
		}
		else {
			turn += angle;
		}
	}

	else if (a == 2) {
		if (b == 3) {
			turn += angle;
		}
		else {
			turn -= angle;
		}
	}

	else if (a == 3) {
		if (b == 1) {
			turn += angle;
		}
		else {
			turn -= angle;
		}
	}

	else if (a == 4) {
		if (b == 1) {
			turn -= angle;
		}
		else {
			turn += angle;
		}
	}
}

void idle()
{
	g_time += 1;

	if (Navigating) {
		maxcount = navigateArrow.size() * 10;

		if (cnt == 0) {
			cameraPos = getPositionFromIndex(Goal[Goal.size() - 1].index.second, Goal[Goal.size() - 1].index.first);
			viewDirection = navigateArrow[0];
			navigator = SetNaviArrow(navigateArrow[0]);
			SetAngle(SetNaviArrow(navigateArrow[0]));
		}

		if (navigator != SetNaviArrow(navigateArrow[cnt / 10])) {
			CheckTurnDirection(navigator, SetNaviArrow(navigateArrow[cnt / 10]));

			viewDirection.x = cos(turn);
			viewDirection.z = sin(turn);

			turncount++;

			if (turncount > 9) {
				turncount = 0;
				navigator = SetNaviArrow(navigateArrow[cnt / 10]);
			}
		}

		else {
			cameraPos += navigateArrow[cnt / 10] * cameraSpeed;
			cnt++;

			if (cnt >= maxcount) {
				cnt = 0;
				Navigating = false;
			}
		}

	}
	else {
		if ((GetAsyncKeyState('A') & 0x8000) == 0x8000) {	// if "A" key is pressed	: Go Left
			LeftTurn = true;

			viewDirection.x = cos(turn);
			viewDirection.z = sin(turn);
		}
		if ((GetAsyncKeyState('D') & 0x8000) == 0x8000) {	// if "D" key is pressed	: Go Right
			RightTurn = true;

			viewDirection.x = cos(turn);
			viewDirection.z = sin(turn);
		}
		if ((GetAsyncKeyState('W') & 0x8000) == 0x8000) {	// if "W" key is pressed	: Go Forward

			cameraPos += cameraSpeed * viewDirection;

			if (CheckCollision(cameraPos + cameraSpeed * viewDirection))
				cameraPos -= cameraSpeed * viewDirection;
		}
		if ((GetAsyncKeyState('S') & 0x8000) == 0x8000) {	// if "S" key is pressed	: Go Backward
			cameraPos += cameraSpeed * viewDirection * -1;
			if (CheckCollision(cameraPos + cameraSpeed * viewDirection * -1))
				cameraPos -= cameraSpeed * viewDirection * -1;
		}
	}

	if ((GetAsyncKeyState(' ') & 0x8000) == 0x8000) {
		Navigate();
	}
	if ((GetAsyncKeyState('Q') & 0x8000) == 0x8000) {
		astar();
	}
	Sleep(16);												// for vSync
	glutPostRedisplay();
}

void reshape(int wx, int wy)
{
	printf("%d %d \n", wx, wy);
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}