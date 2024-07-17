#include "Angel.h"

const int n_divisions = 5;
const int n_tetrahedrons = 1024; // 4^5 tetrahedrons
const int n_triangles = 4 * n_tetrahedrons;
const int n_points = 3 * n_triangles;
vec3 points[n_points];
vec3 colors[n_points]; // A color for each vertex
int index = 0;
int color_index = 0;

void triangle(const vec3 &a, const vec3 &b, const vec3 &c)
{
	static vec3 base_colors[] = {vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), vec3(0.0)};

	points[index] = a;
	colors[index] = base_colors[color_index];
	index++;
	points[index] = b;
	colors[index] = base_colors[color_index];
	index++;
	points[index] = c;
	colors[index] = base_colors[color_index];
	index++;
}

void tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d)
{
	color_index = 0;
	triangle(a, b, c);
	color_index = 1;
	triangle(a, c, d);
	color_index = 2;
	triangle(a, d, b);
	color_index = 3;
	triangle(b, d, c);
}

void divide_tetra(const vec3& a, const vec3& b, const vec3& c, const vec3& d, int divisions) {
	if (divisions > 0) {
		// compute midpoints of sides
		vec3 midpoints[6];
		midpoints[0] = (a + b) / 2.0;
		midpoints[1] = (a + c) / 2.0;
		midpoints[2] = (a + d) / 2.0;
		midpoints[3] = (b + c) / 2.0;
		midpoints[4] = (b + d) / 2.0;
		midpoints[5] = (c + d) / 2.0;

		// create 4 tetrahedrons with the subdivided sides
		divide_tetra(a, midpoints[0], midpoints[1], midpoints[2], divisions - 1);
		divide_tetra(midpoints[0], b, midpoints[3], midpoints[4], divisions - 1);
		divide_tetra(midpoints[1], midpoints[3], c, midpoints[5], divisions - 1);
		divide_tetra(midpoints[2], midpoints[4], midpoints[5], d, divisions - 1);
	}
	else {
		tetra(a, b, c, d);
	}
}

void init(void) {
	vec3 vertices[] = {
		vec3(0.0, 0.0, -1.0),
		vec3(0.0, 0.942809, 0.333333),
		vec3(-0.816497, -0.471405, 0.333333),
		vec3(0.816497, -0.471405, 0.333333)
	};

	divide_tetra(vertices[0], vertices[1], vertices[2], vertices[3], n_divisions);

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), points, GL_STATIC_DRAW);

	// Break up the buffers to designate the colors vs verteices
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader24.glsl", "fshader24.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader
	GLuint loc = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Initialize the vertex color attribute from the vertex shader
	GLuint color_loc = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(color_loc);
	glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear the window
	glDrawArrays(GL_TRIANGLES, 0, n_points);    // draw the points
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("tetrahedron Gasket");
	glewInit();
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}