#include <iostream>
#include <windows.h> 
#include <math.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>

using namespace std;

const int screenWidth = 640;
const int screenHeight = 640;

int width = screenWidth;
int height = screenHeight;

double Wl, Wr, Wb, Wt;

int sides = 100;

double endT = 0;

bool goForward = true;

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

	Wl = -6;
	Wr = 6;
	Wb = -6;
	Wt = 6;
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	setupWorldWindow(Wl, Wr, Wb, Wt);
	glViewport(0, 0, width, height);

	glBegin(GL_LINE_STRIP);
	for (double t = 0.0; t <= endT; t = t + 1.0 / sides)
	{
		double x = 5 * cos(2 * 3.14159265358979323846 * t);
		double y = 5 * sin(2 * 3.14159265358979323846 * t);

		glVertex2d(x, y);
	}
	glEnd();

	glFlush();                 // send all output to display 

	glutSwapBuffers(); // Move to the second buffer
}

void myMouse(int button, int state, int x, int y)
{

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == '=')
	{
		sides = sides + 1;
	}

	if (key == '-')
	{
		sides = sides - 1;
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
	if(goForward == true)
	{
		endT = endT + 0.0001;

		if (endT > 1.1)
			goForward = false;
	}
	else
	{
		endT = endT - 0.0001;

		if (endT <= 0)
			goForward = true;
	}
	

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
