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

vector<Point> points;
int MidX = 320;
int MidY = 240;

double Sx = 1.0, Sy = 1.0;

void UpdateScaling()
{
	for (size_t i = 0; i < points.size(); i++)
	{
		// Translate by -Midx, -Midy

		points[i].x = points[i].x - MidX;
		points[i].y = points[i].y - MidY;

		//Scale
		points[i].x = Sx * points[i].x;
		points[i].y = Sy * points[i].y;

		// Translate by MidX, MidY

		points[i].x = points[i].x + MidX;
		points[i].y = points[i].y + MidY;
	}
}

void setupWorldWindow(double left, double right,
	double	bottom, double top)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, right, bottom, top);
}

//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
	glColor3f(0.0f, 0.0f, 0.0f);          // set the drawing color 
	glPointSize(8.0);       // a ‘dot’ is 4 by 4 pixels

	points.push_back(Point(220, 140));
	points.push_back(Point(420, 140));
	points.push_back(Point(420, 340));
	points.push_back(Point(220, 340));
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	setupWorldWindow(0, screenWidth, 0, screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
	
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	for (size_t i = 0; i < points.size(); i++)
	{		
		glVertex2d(points[i].x, points[i].y);
		
	}
	glEnd();

	glColor3f(1, 0, 0);

	glBegin(GL_POINTS);
	for (size_t i = 0; i < points.size(); i++)
	{
		glVertex2d(points[i].x, points[i].y);

	}
	glVertex2d(MidX, MidY);
	glEnd();

	glFlush();                 // send all output to display 

	glutSwapBuffers(); // Move to the second buffer
}

void myMouse(int button, int state, int x, int y)
{
	y = screenHeight - y;



	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == '-')
	{
		Sx = 0.9;
		Sy = 0.9;
	}

	if (key == '=')
	{
		Sx = 1.1;
		Sy = 1.1;
	}

	UpdateScaling();

	glutPostRedisplay();
}

void myDraggedMouse(int x, int y)
{

	glutPostRedisplay();
}

void myPassiveMouse(int x, int y)
{
}

void myReshape(int newWidth, int newHeight)
{
	glutPostRedisplay();
}

void myIdle()
{

	glutPostRedisplay();
}

//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char** argv)
{
	glutInit(&argc, argv);          // initialize the toolkit
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE); // set display mode
	glutInitWindowSize(screenWidth, screenHeight);     // set window size
	glutInitWindowPosition(100, 100); // set window position on screen
	glutCreateWindow("My Graphics App"); // open the screen window
	glutDisplayFunc(myDisplay);     // register redraw function
	glutMouseFunc(myMouse); // register the mouse function
	glutKeyboardFunc(myKeyboard); // register the keyboard function
	glutMotionFunc(myDraggedMouse); // register the mouse drag
	glutPassiveMotionFunc(myPassiveMouse); // register the passive mouse move

	glutReshapeFunc(myReshape); // register the reshape function
	glutIdleFunc(myIdle); // register the idle function

	myInit();
	glutMainLoop();      // go into a perpetual loop
}
