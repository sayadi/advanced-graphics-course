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

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-4.0, 4.0, -4.0 * (double)screenHeight / screenWidth, 4.0 * (double)screenHeight / screenWidth, -4.0, 4.0);
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

	glColor3f(0, 1, 0);

	glBegin(GL_TRIANGLES);
	glVertex3d(0, 0, 0);
	glVertex3d(3, 0, 0);
	glVertex3d(3, 2, 3);
	glEnd();

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
