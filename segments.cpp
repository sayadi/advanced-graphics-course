#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include <vector>

#include "vector.h"

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

vector<Point> points;

bool Intersect(Point A, Point B, Point C, Point D, Point& I)
{
    Vector b = B - A;
    Vector d = D - C;
    Vector bPerp = perp(b);
    Vector dPerp = perp(d);
    Vector c = C - A;

    double t = (c % dPerp) / (b % dPerp);
    double u = -(c % bPerp) / (d % bPerp);

    I = A + t * b;

    return t >= 0 && t <= 1 && u >= 0 && u <= 1;
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
    glPointSize(4.0);       // a �dot� is 4 by 4 pixels
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

    // Write your drawing code here

    setupWorldWindow(0, screenWidth, 0, screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glColor3f(1, 0, 0);

    if (points.size() >= 2)
    {
        glBegin(GL_LINES);
        glVertex2d(points[0].x, points[0].y);
        glVertex2d(points[1].x, points[1].y);
        glEnd();
    }

    if (points.size() >= 4)
    {
        glBegin(GL_LINES);
        glVertex2d(points[2].x, points[2].y);
        glVertex2d(points[3].x, points[3].y);
        glEnd();
    }

    glColor3d(0, 0, 0);
    glBegin(GL_POINTS);
    for (int i = 0; i < points.size(); i++)
    {
        Point p = points[i];
        glVertex2d(p.x, p.y);
    }
    glEnd();

    if (points.size() >= 4)
    {
        Point I;

        bool didIntersect = Intersect(points[0],
                                      points[1],
                                      points[2],
                                      points[3],
                                      I);

        if (didIntersect == true)
            glColor3d(0, 1, 0);
        else
            glColor3d(1, 0, 1);
        glBegin(GL_POINTS);
        glVertex2d(I.x, I.y);
        glEnd();
    }


    glFlush();                 // send all output to display

    glutSwapBuffers(); // Move to the second buffer
}

void myMouse(int button, int state, int x, int y)
{
    y = screenHeight - y;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && points.size() < 4)
    {
        Point p = Point(x, y);

        points.push_back(p);

        cout << "Total points: " << points.size() << endl;
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && points.size() > 0)
    {
        points.clear();
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
int main(int argc, char** argv)
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

    return 0;
}
