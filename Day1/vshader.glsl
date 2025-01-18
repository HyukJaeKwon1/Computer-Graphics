#version 330

uniform int wave;
uniform float uTime;

in vec4 vPosition;
in vec4 vColor;

out vec4 color;
out vec4 position;

void main()
{
    color = vColor;
    position = vPosition;

    float time = uTime;

    mat4 reverse = mat4(
        -1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, -1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    mat4 m1 = mat4(1.0f);
    float theta = time * 30 / 180.0 * 3.141592;
    float s = sin(theta);
    float c = cos(theta);
    float c1 = 1.0 - c;

    float x = 0.0;
    float y = -sqrt(2) / 2.0;
    float z = -sqrt(2) / 2.0;

    m1[0][0] = c1 * x * x + c;     m1[1][0] = c1 * x * y - z * s;     m1[2][0] = c1 * z * x + y * s;     m1[3][0] = 0.0f;
    m1[0][1] = c1 * x * y + z * s; m1[1][1] = c1 * y * y + c;         m1[2][1] = c1 * y * z - x * s;     m1[3][1] = 0.0f;
    m1[0][2] = c1 * z * x - y * s; m1[1][2] = c1 * y * z + x * s;     m1[2][2] = c1 * z * z + c;         m1[3][2] = 0.0f;
    m1[0][3] = 0.0f;                m1[1][3] = 0.0f;                    m1[2][3] = 0.0f;                    m1[3][3] = 1.0f;

    mat4 m2 = mat4(1.0f);
    float theta2 = -45 / 180.0 * 3.141592;
    float s2 = sin(theta2);
    float c2 = cos(theta2);
    m2[0][0] = 1.0f; m2[1][0] = 0.0f; m2[2][0] = 0.0f; m2[3][0] = 0.0f;
    m2[0][1] = 0.0f; m2[1][1] = c2;   m2[2][1] = -s2; m2[3][1] = 0.0f;
    m2[0][2] = 0.0f; m2[1][2] = s2;   m2[2][2] = c2;  m2[3][2] = 0.0f;
    m2[0][3] = 0.0f; m2[1][3] = 0.0f;  m2[2][3] = 0.0f; m2[3][3] = 1.0f;

    if (wave == 1)
    {
        float x, y;
        x = position.x;
        y = position.y;
        float r = sqrt((x * x) + (y * y));
        float e = exp(-7 * r * r) * cos(r * 30 - 5 * time);
        if (position.x != 0.8 && position.x != -0.8 && position.y != 0.8 && position.y != -0.8)
        {
            position.z = e / 3;
        }
    }

    gl_Position = reverse * m1 * m2 * position;
}
