#version 330

in vec4 color;
in vec4 position;

out vec4 fColor;

void main()
{
	fColor = color;

	if(position.z > 0.0)
		fColor = fColor + vec4(position.z * 3.8, position.z * 1.5, 0.0,1.0);

	else if(position.z < 0.0)
		fColor = fColor + vec4(0.0,-1.5 * position.z,-3.8 * position.z,1.0);
}