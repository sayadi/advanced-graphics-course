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

double angle = 0;

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

void shear2D(double h, double g)
{
	glMatrixMode(GL_MODELVIEW);
	double param[16] = { 1.0, g, 0.0, 0.0,
						h, 1.0, 0.0, 0.0,			                 			      0.0, 0.0, 1.0, 0.0,
						0.0, 0.0, 0.0, 1.0 };
	glMultMatrixd(param);
}

void drawHouse()
{
	glColor3f(0.0, 1.0, 1.0);
	glRecti(-5, -5, 5, 10);

	glColor3f(0.0, 0.0, 1.0);
	glRecti(-2, -5, 2, 5);

	glColor3f(0.0, 1.0, 0.0);
	glRecti(5, -5, 20, 10);

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_TRIANGLES);
	glVertex2i(-5, 10);
	glVertex2i(5, 10);
	glVertex2i(0, 15);
	glEnd();

	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2i(0, 15);
	glVertex2i(5, 10);
	glVertex2i(20, 10);
	glVertex2i(15, 15);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_LINES);
	glVertex2d(-10, 0);
	glVertex2d(30, 0);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_LINES);
	glVertex2d(0, -10);
	glVertex2d(0, 20);
	glEnd();
}


//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	setupWorldWindow(-10, 30, -10, 20);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotated(45, 0, 0, 1);

	glPushMatrix();

	glViewport(0, 0, screenWidth / 2, screenHeight / 2);
	drawHouse();

	glScaled(0.5, 0.5, 1);

	glViewport(screenWidth / 2, screenHeight / 2, screenWidth / 2, screenHeight / 2);	
	drawHouse();

	glPopMatrix();

	glViewport(0, screenHeight / 2, screenWidth / 2, screenHeight / 2);
	drawHouse();

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
		angle += 5;
	}

	if (key == '=')
	{
		angle -= 5;
	}

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
