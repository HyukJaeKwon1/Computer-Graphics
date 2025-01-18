#ifndef _MY_PLAIN_H_
#define _MY_PLAIN_H_

#include<vgl.h>
#include<InitShader.h>
#include<vec.h>
#include<mat.h>

vec4* vertex_pos;
vec4* vertex_col;

class MyPlain {
public:
	int m_numVertices;
	GLuint m_vao;
	GLuint m_vbo;
	GLuint prog;
	vec4* points;
	vec4* colors;
	int m_numPoints, m_numColors;
	int index;

	MyPlain() {
		points = NULL;
		colors = NULL;
	}

	void drawPlain(int a, int b, int c, int d)
	{
		points[index] = vertex_pos[a];	colors[index] = vertex_col[a];	index++;
		points[index] = vertex_pos[b];	colors[index] = vertex_col[a];	index++;
		points[index] = vertex_pos[c];	colors[index] = vertex_col[a];	index++;

		points[index] = vertex_pos[c];	colors[index] = vertex_col[a];	index++;
		points[index] = vertex_pos[d];	colors[index] = vertex_col[a];	index++;
		points[index] = vertex_pos[a];	colors[index] = vertex_col[a];	index++;
	}
	void makePlain(int n) {
		index = 0;

		for (int i = 0; i < (n + 1) * (n + 1); i++)
		{
			if (i < n * (n + 1) && (i - n) % (n + 1) != 0)
			{
				drawPlain(i, i + 1, i + 1 + n + 1, i + n + 1);
			}
		}

	}

	void initPosition(int n) {

		m_numVertices = (n) * (n) * 6;

		points = new vec4[m_numVertices];
		colors = new vec4[m_numVertices];

		m_numPoints = sizeof(vec4) * m_numVertices;
		m_numColors = sizeof(vec4) * m_numVertices;

		vertex_pos = new vec4[(n + 1) * (n + 1)];
		vertex_col = new vec4[(n + 1) * (n + 1)];

		float x = -0.8;
		float y = 0.8;
		float t = 1.6 / (float)n;

		for (int i = 0; i < n + 1; i++)
		{
			for (int j = 0; j < n + 1; j++)
			{
				vertex_pos[(i * (n + 1)) + j].x = x + (t * j);
				vertex_pos[(i * (n + 1)) + j].y = y - (t * i);
				vertex_pos[(i * (n + 1)) + j].z = 0.0;
				vertex_pos[(i * (n + 1)) + j].w = 1;
				if ((i + j) % 2 == 0)
					vertex_col[(i * (n + 1)) + j] = vec4(0.6, 0.6, 0.6, 1.0);
				else
					vertex_col[(i * (n + 1)) + j] = vec4(0.5, 0.5, 0.5, 1.0);
			}
		}
		makePlain(n);
	}

	void init(int n) {
		initPosition(n);

		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_numPoints + m_numColors, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_numPoints, points);
		glBufferSubData(GL_ARRAY_BUFFER, m_numPoints, m_numColors, colors);

		prog = InitShader("vshader.glsl", "fshader.glsl");
		glUseProgram(prog);

		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, false, 0, BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, false, 0, BUFFER_OFFSET(m_numPoints));
	}

	void Redraw(int n)
	{
		index = 0;
		initPosition(n);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_numPoints + m_numColors, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_numPoints, points);
		glBufferSubData(GL_ARRAY_BUFFER, m_numPoints, m_numColors, colors);

		glUseProgram(prog);

		GLuint vPosition = glGetAttribLocation(prog, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, false, 0, BUFFER_OFFSET(0));

		GLuint vColor = glGetAttribLocation(prog, "vColor");
		glEnableVertexAttribArray(vColor);
		glVertexAttribPointer(vColor, 4, GL_FLOAT, false, 0, BUFFER_OFFSET(m_numPoints));

		glDrawArrays(GL_TRIANGLES, 0, m_numVertices);

	}

	void draw(float time, int n) {
		glBindVertexArray(m_vao);
		glUseProgram(prog);

		GLuint uTime = glGetUniformLocation(prog, "uTime");
		glUniform1f(uTime, time);

		GLuint wave = glGetUniformLocation(prog, "wave");
		glUniform1i(wave, n);


		glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
	}
};

#endif
