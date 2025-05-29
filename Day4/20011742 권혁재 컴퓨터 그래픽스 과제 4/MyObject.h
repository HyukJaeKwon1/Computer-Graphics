#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vgl.h>
#include <vec.h>
#include <limits.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct MyObjectVertex
{
	vec4 position;
	vec4 color;
	vec3 normal;
};

class MyObject
{
public:
	MyObject(void);
	~MyObject(void);

	int OpenObject(std::string& objName);
	void CloseObject();
	vec2 CountData();

	void FlatShading();
	void PhongShading();

	int NumVertices;

	vec3* verticeArray;
	vec3* normalArrayFlat;
	vec3* normalArrayPhong;

	MyObjectVertex* Vertices;
	int* verticesIdx;
	GLuint Init(std::string objName, vec4 color = vec4(0.79, 0.89, 0.84, 1));
	void SetPositionAndOtherAttributes(GLuint program);

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void Draw(GLuint program);
	vec3 maxXYZ = vec3(INT_MIN, INT_MIN, INT_MIN);
	vec3 minXYZ = vec3(INT_MAX, INT_MAX, INT_MAX);
	vec3 sizeXYZ = vec3(2, 2, 2);
	vec3 center = vec3(0, 0, 0);
};

MyObject::MyObject(void)
{
	bInitialized = false;
	NumVertices = 0;
	Vertices = NULL;
}

MyObject::~MyObject(void)
{
	if (Vertices != NULL)
		delete[] Vertices;
	if (verticesIdx != NULL)
		delete[] verticesIdx;
	if (verticeArray != NULL)
		delete[] verticeArray;
	if (normalArrayFlat != NULL)
		delete[] normalArrayFlat;
	if (normalArrayPhong != NULL)
		delete[] normalArrayPhong;
}

std::ifstream File;

int MyObject::OpenObject(std::string& objName) {
	File.open(objName);
	if (!File.is_open()) {
		std::cout << "File not Found!" << std::endl;
		return -1;
	}
	return 0;
}
vec2 MyObject::CountData() {
	vec2 cnt = vec2(0, 0);

	std::string Line;
	while (std::getline(File, Line)) {
		std::istringstream iss(Line);
		std::string code;
		iss >> code;

		if (code == "v") {
			cnt.x++;
		}
		if (code == "f") {
			cnt.y++;
		}
	}
	return cnt;
}

void MyObject::CloseObject() {
	File.close();
}

GLuint MyObject::Init(std::string objName, vec4 color)
{
	vec2 cnt = CountData();
	NumVertices = cnt.y * 3;
	verticeArray = new vec3[cnt.x];
	normalArrayPhong = new vec3[cnt.x];
	normalArrayFlat = new vec3[NumVertices];
	Vertices = new MyObjectVertex[NumVertices];
	verticesIdx = new int[NumVertices];
	CloseObject();

	OpenObject(objName);

	std::string line;

	int i = 0, j = 0;
	while (std::getline(File, line)) {
		std::istringstream iss(line);
		std::string code;
		iss >> code;

		if (code == "v") {
			vec3 vertex;
			sscanf(line.c_str(), "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
			verticeArray[i] = vertex;

			if (maxXYZ.x < vertex.x) maxXYZ.x = vertex.x;
			if (maxXYZ.y < vertex.y) maxXYZ.y = vertex.y;
			if (maxXYZ.z < vertex.z) maxXYZ.z = vertex.z;

			if (minXYZ.x > vertex.x) minXYZ.x = vertex.x;
			if (minXYZ.y > vertex.y) minXYZ.y = vertex.y;
			if (minXYZ.z > vertex.z) minXYZ.z = vertex.z;

			i++;
		}
		else if (code == "f") {
			int vData1, vData2, vData3;
			sscanf(line.c_str(), "f %d %d %d", &vData1, &vData2, &vData3);

			vec3 v1 = verticeArray[vData1 - 1];
			vec3 v2 = verticeArray[vData2 - 1];
			vec3 v3 = verticeArray[vData3 - 1];

			vec3 nv = normalize(cross((v2 - v1), (v3 - v1)));

			normalArrayPhong[vData1 - 1] += nv;
			normalArrayPhong[vData2 - 1] += nv;
			normalArrayPhong[vData3 - 1] += nv;

			Vertices[j].position = v1;	Vertices[j].color = color; normalArrayFlat[j] = nv; verticesIdx[j] = vData1 - 1; j++;
			Vertices[j].position = v2;	Vertices[j].color = color; normalArrayFlat[j] = nv; verticesIdx[j] = vData2 - 1; j++;
			Vertices[j].position = v3;	Vertices[j].color = color; normalArrayFlat[j] = nv; verticesIdx[j] = vData3 - 1; j++;

		}
	}

	for (int j = 0; j < NumVertices; j++)
		Vertices[j].normal = normalize(normalArrayPhong[verticesIdx[j]]);


	CloseObject();

	if (bInitialized == true) return vao;

	sizeXYZ = vec3(maxXYZ - minXYZ);
	center = (maxXYZ + minXYZ) / 2;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex) * NumVertices, Vertices, GL_STATIC_DRAW);

	bInitialized = true;
	return vao;
}

void MyObject::FlatShading() {
	for (int j = 0; j < NumVertices; j++)
		Vertices[j].normal = normalArrayFlat[j];

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex) * NumVertices, Vertices, GL_STATIC_DRAW);
}

void MyObject::PhongShading() {
	for (int j = 0; j < NumVertices; j++)
		Vertices[j].normal = normalize(normalArrayPhong[verticesIdx[j]]);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex) * NumVertices, Vertices, GL_STATIC_DRAW);
}

void MyObject::SetPositionAndOtherAttributes(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4) + sizeof(vec4)));


}


void MyObject::Draw(GLuint program)
{
	if (!bInitialized) return;

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndOtherAttributes(program);

	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}