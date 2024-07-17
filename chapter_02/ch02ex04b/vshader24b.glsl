#version 150

in vec4 vPosition;
out vec4 color;

void main()
{
	gl_Position = vPosition;
	color = vec4((vec3(1.0) + vPosition.xyz) / 2.0, 1.0);
}