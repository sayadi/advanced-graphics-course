#include <iostream>
#include <windows.h> 
#include <math.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

int mX1, mY1, mX2, mY2, mX3, mY3;
int num_clicks = 0;

const int RED = 1;
const int GREEN = 2;
const int BLUE = 3;

double r = 0;
double g = 0;
double b = 0;

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

	glColor3d(r, g, b);
	if (num_clicks >= 2)
	{
		glBegin(GL_LINES);
		glVertex2i(mX1, mY1);
		glVertex2i(mX2, mY2);
		glEnd();
	}

	if (num_clicks == 3)
	{
		glBegin(GL_TRIANGLES);
		glVertex2i(mX1, mY1);
		glVertex2i(mX2, mY2);
		glVertex2i(mX3, mY3);
		glEnd();
	}

	glColor3f(1, 0, 1);	

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

	if (num_clicks >= 3)
	{
		glBegin(GL_POINTS);
		glVertex2i(mX3, mY3);
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
		else if (num_clicks == 2)
		{
			mX3 = x;
			mY3 = y;
			num_clicks++;
		}
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		cout << "Left button released" << endl;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		cout << "Right button pressed" << endl;

		if (num_clicks > 0)
		{
			num_clicks--;
		}
	}

	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 'Q' || key == 'q')
	{
		exit(1);
	}

	if (key == '1')
	{
		cout << "1 is pressed" << endl;
	}

	if (key == 'a' || key == 'A')
	{
		cout << "A or a is pressed" << endl;
	}
}

void myDraggedMouse(int x, int y)
{
	y = screenHeight - y;

	//cout << "Drag: " << x << ", " << y << endl;

	if (num_clicks == 1)
	{
		mX1 = x;
		mY1 = y;
	}
	else if (num_clicks == 2)
	{
		mX2 = x;
		mY2 = y;
	}
	else if (num_clicks == 3)
	{
		mX3 = x;
		mY3 = y;
	}

	glutPostRedisplay();
}

void myPassiveMouse(int x, int y)
{
	y = screenHeight - y;
	//cout << "Passive move: " << x << ", " << y << endl;
}

void myMenu(int num)
{
	if (num == RED)
	{
		r = 1;
		g = 0;
		b = 0;
	}
	else if (num == GREEN)
	{
		r = 0;
		g = 1;
		b = 0;
	}
	else if (num == BLUE)
	{
		r = 0;
		g = 0;
		b = 1;
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

	glutCreateMenu(myMenu);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Blue", BLUE);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	myInit();
	glutMainLoop();      // go into a perpetual loop
}
