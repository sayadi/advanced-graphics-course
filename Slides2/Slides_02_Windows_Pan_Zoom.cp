#include <iostream>
#include <windows.h> 
#include <math.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

int width = screenWidth;
int height = screenHeight;

double Wl, Wr, Wb, Wt;

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
	glPointSize(4.0);       // a ‘dot’ is 4 by 4 pixels

	Wl = -10;
	Wr = 30;
	Wb = -10;
	Wt = 20;
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	setupWorldWindow(Wl, Wr, Wb, Wt);
	glViewport(0, 0, width, height);
	drawHouse();

	glFlush();                 // send all output to display 
}

void myMouse(int button, int state, int x, int y)
{

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 'a')
	{
		Wl += 1;
		Wr += 1;
	}

	if (key == 'd')
	{
		Wl -= 1;
		Wr -= 1;
	}

	if (key == 'w')
	{
		Wb -= 1;
		Wt -= 1;
	}

	if (key == 's')
	{
		Wb += 1;
		Wt += 1;
	}

	if (key == '=')
	{
		double aspect = 4.0 / 3.0;

		Wl += 1;
		Wr -= 1;
		Wb += 1 / aspect;
		Wt -= 1 / aspect;
	}

	if (key == '-')
	{
		double aspect = 4.0 / 3.0;

		Wl -= 1;
		Wr += 1;
		Wb -= 1 / aspect;
		Wt += 1 / aspect;
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
	cout << "New: " << newWidth << ", " << newHeight << endl;

	double aspect = 4.0 / 3.0;

	if (newWidth < newHeight)
	{
		width = newWidth;
		height = width / aspect;
	}
	else
	{
		height = newHeight;
		width = aspect * height;
	}

	glutPostRedisplay();
}

//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
void main(int argc, char** argv)
{
	glutInit(&argc, argv);          // initialize the toolkit
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_MULTISAMPLE); // set display mode
	glutInitWindowSize(screenWidth, screenHeight);     // set window size
	glutInitWindowPosition(100, 100); // set window position on screen
	glutCreateWindow("My Graphics App"); // open the screen window
	glutDisplayFunc(myDisplay);     // register redraw function
	glutMouseFunc(myMouse); // register the mouse function
	glutKeyboardFunc(myKeyboard); // register the keyboard function
	glutMotionFunc(myDraggedMouse); // register the mouse drag
	glutPassiveMotionFunc(myPassiveMouse); // register the passive mouse move

	glutReshapeFunc(myReshape); // register the reshape function

	myInit();
	glutMainLoop();      // go into a perpetual loop
}
