#include <iostream>
#include <windows.h> 
#include <math.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

int mX1, mY1, mX2, mY2;
int num_clicks = 0;

//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
	glColor3f(0.0f, 0.0f, 0.0f);          // set the drawing color 
	glPointSize(4.0);       // a ‘dot’ is 4 by 4 pixels
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	// Write your drawing code here

	glColor3f(0, 1, 0);
	if (num_clicks >= 2)
	{
		glBegin(GL_LINES);
		glVertex2i(mX1, mY1);
		glVertex2i(mX2, mY2);
		glEnd();
	}

	glColor3f(1, 0, 0);	

	if (num_clicks >= 1)
	{
		glBegin(GL_POINTS);
		glVertex2i(mX1, mY1);
		glEnd();
	}
	
	if (num_clicks >= 2)
	{
		glBegin(GL_POINTS);
		glVertex2i(mX2, mY2);
		glEnd();
	}


	glFlush();                 // send all output to display 
}

void myMouse(int button, int state, int x, int y)
{
	y = screenHeight - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		cout << "Left button Pressed" << endl;

		if (num_clicks == 0)
		{
			mX1 = x;
			mY1 = y;
			num_clicks++;
		}
		else if (num_clicks == 1)
		{
			mX2 = x;
			mY2 = y;
			num_clicks++;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		cout << "Left button released" << endl;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		cout << "Right button" << endl;
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
	myInit();
	glutMainLoop();      // go into a perpetual loop
}
