#version 150

in vec4 vPosition;
out vec4 color;

void
main()
{
    color = vec4((vPosition.xyz + 1.0) / 2.0, 1.0);
    gl_Position = vPosition;
}
