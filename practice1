#include <iostream>
#include <vector>
#include <fstream>
#include <GL/glut.h>
#include <ctime>
using namespace std;

vector<float> triangle, quadrangle;
bool bDrawTri = true;

float  g(int min, int max) {
	static const double fraction = 1.0 / (RAND_MAX + 1.0);
	return min + (max - min + 1) * (rand() * fraction);
}

void readFile() {
	/* Implement: read two files and store the data */
	ifstream ifst("triangle.txt"), ifsq("quadrangle.txt");
	float val;
	while (ifst >> val, !ifst.eof()) {
		triangle.push_back(val);
	}

	while (ifsq >> val, !ifsq.eof()) {
		triangle.push_back(val);
	}

}

void drawPrimitive() {
	/* Implement: draw a triangle or a quadrangle */
	if (bDrawTri) {
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < triangle.size(); i += 2) {
			glColor3f(g(0,1), g(0, 1), g(0, 1));
			glVertex2f(triangle[i], triangle[i + 1]);
		}
		glEnd();
	}

	else {
		glBegin(GL_QUADS);
		for (int j = 0; j < quadrangle.size(); j += 2) {
			glColor3f(g(0, 1), g(0, 1), g(0, 1));
			glVertex2f(quadrangle[j], quadrangle[j + 1]);
		}
		glEnd();
	}
}

void renderScene() {
	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	drawPrimitive();

	glFlush();
	//glFinish();
}

void processNormalKeys(unsigned char key, int x, int y) {
	printf("You pressed %c at (%d, %d)\n", key, x, y);
	if (key == 27)
		exit(0);
	else if (key == 't' || key == 'T')
		bDrawTri = true;
	else if (key == 'q' || key == 'Q')
		bDrawTri = false;

	glutPostRedisplay();
}

void main(int argc, char** argv) {
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 640);
	glutCreateWindow("Draw a triangle or a quadrangle");

	// read file
	readFile();

	// register callbacks
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(processNormalKeys);

	// enter GLUT event processing cycle
	glutMainLoop();
}
