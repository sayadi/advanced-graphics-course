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
	glPointSize(4.0);       // a ‘dot’ is 4 by 4 pixels
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	setupWorldWindow(0, screenWidth, 0, screenHeight);
	glViewport(0, 0, screenWidth, screenHeight);
	
	glColor3d(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < points.size(); i++)
	{
		Point p = points[i];
		glVertex2d(p.x, p.y);

		if (points.size() > 1 && i < points.size() - 1)
		{
			Point next = points[i + 1];
			Point Mid = 0.5 * p + 0.5 * next;
		}
	}
	glEnd();

	glColor3d(0, 0, 0);
	glBegin(GL_POINTS);
	for (int i = 0; i < points.size(); i++)
	{
		Point p = points[i];
		glVertex2d(p.x, p.y);
	}
	glEnd();

	for (int i = 0; i < points.size(); i++)
	{
		Point p = points[i];
		glVertex2d(p.x, p.y);

		if (points.size() > 1 && i < points.size() - 1)
		{
			Point next = points[i + 1];
			Point Mid = 0.5 * p + 0.5 * next;

			Vector v = next - p;
			Vector n = perp(v);

			Point perpEnd = Mid + n;

			glColor3d(1, 0, 1);
			glBegin(GL_LINES);
			glVertex2d(Mid.x, Mid.y);
			glVertex2d(perpEnd.x, perpEnd.y);
			glEnd();

			glColor3d(0, 0, 1);
			glBegin(GL_POINTS);
			glVertex2d(Mid.x, Mid.y);
			glVertex2d(perpEnd.x, perpEnd.y);
			glEnd();
		}
	}

	glFlush();                 // send all output to display 

	glutSwapBuffers(); // Move to the second buffer
}

void myMouse(int button, int state, int x, int y)
{
	y = screenHeight - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		Point p = Point(x, y);

		points.push_back(p);

		cout << "Total points: " << points.size() << endl;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && points.size() > 0)
	{
		points.pop_back();
		cout << "Total points: " << points.size() << endl;
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
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
