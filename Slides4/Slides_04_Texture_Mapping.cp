#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h> 
#include <math.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>
#include <vector>

#include "vector.h"

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

Point P1, P2;
int num_clicks = 0;

double angleX = 0, angleY = 0;

float* depth;

//Texture variables

unsigned char* image = 0;
GLuint		texture[2];
int image_w, image_h;

unsigned char* readPPM(const char* filename, bool flag, int& dimx, int& dimy) {
	FILE* ppmfile;
	char          line[256];
	int           i, pixels, x, y, r, g, b;
	unsigned char* p;
	unsigned char* f;

	if ((ppmfile = fopen(filename, "rb")) == NULL) {
		printf("can't open %s\n", filename);
		exit(1);
	}

	fgets(line, 255, ppmfile);
	fgets(line, 255, ppmfile);
	while (line[0] == '#' || line[0] == '\n') fgets(line, 255, ppmfile);
	sscanf(line, "%d %d", &dimx, &dimy);
	fgets(line, 255, ppmfile);

	pixels = dimx * dimy;
	p = (unsigned char*)calloc(3 * pixels, sizeof(unsigned char));
	f = (unsigned char*)calloc(3 * pixels, sizeof(unsigned char));
	// 3 * pixels because of R, G and B channels
	i = 0;
	for (y = 0; y < dimy; y++) {
		for (x = 0; x < dimx; x++) {
			i = 3 * x + y * (3 * dimx);
			r = getc(ppmfile);
			p[i] = r;
			g = getc(ppmfile);
			p[i + 1] = g;
			b = getc(ppmfile);
			p[i + 2] = b;
		}
	}
	fclose(ppmfile);

	unsigned char* ptr1, * ptr2;

	ptr1 = p;
	ptr2 = f + 3 * dimx * (dimy - 1);
	for (y = 0; y < dimy; y++) {
		for (x = 0; x < dimx * 3; x++) {
			*ptr2 = *ptr1;
			ptr1++;
			ptr2++;
		}
		ptr2 -= (2 * 3 * dimx);
	}

	if (!flag) {
		free(p);
		p = 0;
		return(f);
	}
	else {
		free(f);
		f = 0;
		return(p);
	}

	return 0;
}

Point get_3D_pos(int x, int y)
{
	GLdouble pMatrix[16], mMatrix[16];

	glGetDoublev(GL_PROJECTION_MATRIX, pMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, mMatrix);

	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	y = viewport[3] - y;

	int winWidth = viewport[2];
	int winHeight = viewport[3];

	if (depth)
	{
		delete[] depth;
		depth = 0;
	}

	depth = new float[screenWidth * screenHeight];
	glReadPixels(0, 0, screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_FLOAT, depth);



	float z = depth[y * winWidth + x];

	cout << "Z = " << z << endl;

	if (z >= 1.0f - 1e-12 || z <= 1e-12)
		z = 0.5f;

	GLdouble wx = x;
	GLdouble wy = y;
	GLdouble wz = z;

	GLdouble px, py, pz;

	gluUnProject(wx, wy, wz, mMatrix, pMatrix, viewport, &px, &py, &pz);

	Point p = { (float)px, (float)py, (float)pz };

	cout << "Point = " << p << endl;

	return p;
}

//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
	glColor3f(0.0f, 0.0f, 0.0f);          // set the drawing color 
	glPointSize(4.0);       // a ‘dot’ is 4 by 4 pixels

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-4.0, 4.0, -4.0 * (double)screenHeight / screenWidth, 4.0 * (double)screenHeight / screenWidth, -4.0, 4.0);

	

	glGenTextures(2, &texture[0]);

	image = readPPM("UAE.ppm", false, image_w, image_h);

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	if (image)
		free(image);

	image = 0;

	image = readPPM("SIDE_F.ppm", false, image_w, image_h);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	if (image)
		free(image);

	image = 0;
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotated(angleX, 1, 0, 0);
	glRotated(angleY, 0, 1, 0);

	glColor3f(0, 0, 0);

	glBegin(GL_LINES);
	glVertex3d(-4, 0, 0);
	glVertex3d(4, 0, 0);
	glEnd();


	glColor3f(1, 0, 0);

	glBegin(GL_LINES);
	glVertex3d(0, -3, 0);
	glVertex3d(0, 3, 0);
	glEnd();

	glColor3f(0, 0, 1);

	glBegin(GL_LINES);
	glVertex3d(0, 0, -4);
	glVertex3d(0, 0, 4);
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glColor3f(1, 1, 1);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3d(0, 0, 0);

	glTexCoord2f(1, 0);
	glVertex3d(3, 0, 0);

	glTexCoord2f(1, 1);
	glVertex3d(3, 2, 3);

	glTexCoord2f(0, 1);
	glVertex3d(0, 2, 3);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glBegin(GL_TRIANGLES);
	glTexCoord2f(0, 0);
	glVertex3d(0, 0, 0);

	glTexCoord2f(1, 0);
	glVertex3d(-3, 0, 0);

	glTexCoord2f(1, 1);
	glVertex3d(-3, 2, 3);

	glEnd();


	glDisable(GL_TEXTURE_2D);

	if (num_clicks >= 2)
	{
		glColor3d(0.5, 0.5, 0.5);
		glBegin(GL_LINES);
		glVertex3d(P1.x, P1.y, P1.z);
		glVertex3d(P2.x, P2.y, P2.z);
		glEnd();
	}

	if (num_clicks >= 1)
	{
		glColor3f(1, 0, 1);

		glBegin(GL_POINTS);
		glVertex3d(P1.x, P1.y, P1.z);
		glEnd();
	}

	if (num_clicks >= 2)
	{
		glColor3f(1, 0, 1);

		glBegin(GL_POINTS);
		glVertex3d(P2.x, P2.y, P2.z);
		glEnd();
	}

	glutSwapBuffers();

	glFlush();                 // send all output to display 
}

void myResize(int resizeWidth, int resizeHeight)
{
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 'w')
		angleX += 5;

	if (key == 's')
		angleX -= 5;

	if (key == 'a')
		angleY += 5;

	if (key == 'd')
		angleY -= 5;

	glutPostRedisplay();
}

void myIdle()
{

	glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (num_clicks == 0)
		{
			P1 = get_3D_pos(x, y);
			num_clicks++;
		}
		else if (num_clicks == 1)
		{
			P2 = get_3D_pos(x, y);
			num_clicks++;
		}
	}

}

//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char** argv)
{
	glutInit(&argc, argv);          // initialize the toolkit
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE); // set display mode

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);



	glutInitWindowSize(screenWidth, screenHeight);     // set window size
	glutInitWindowPosition(100, 100); // set window position on screen
	glutCreateWindow("My Graphics App"); // open the screen window

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(myDisplay);     // register redraw function


	glutReshapeFunc(myResize);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);
	glutMouseFunc(myMouse);

	myInit();
	glutMainLoop();      // go into a perpetual loop

	return 0;
}
