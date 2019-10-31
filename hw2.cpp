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

/* ********** ********** ********** */

/* ********** Utility functions ********** */

/* ********** ********** ********** */

/* ********** OpenGL functions ********** */

void setupWorldWindow(double left, double right,
                      double	bottom, double top)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, right, bottom, top);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupWorldWindow(0, screenWidth, 0, screenHeight);
    glViewport(0, 0, screenWidth, screenHeight);

    glColor3d(1, 0, 0);
    glPointSize(4.0);

    glBegin(GL_POINTS);
    for (auto & point : points)
    {
        glVertex2d(point.x, point.y);
    }
    glEnd();
}

void myDraggedMouse(int x, int y)
{

    y = screenHeight - y;

    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    y = screenHeight - y;

    bool rightDown = button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN;
    if (rightDown)
    {
        Point p = Point(x, y);
        points.push_back(p);

        cout << "Total points: " << points.size() << endl;
    }

    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
    glutPostRedisplay();
}

void idle()
{
    glutPostRedisplay();
}

void CreateGlutWindow()
{
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize (screenWidth, screenHeight);
    glutCreateWindow ("Assignment 1 - Mohamad Al Sayadi");
}

void CreateGlutCallbacks()
{
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutKeyboardFunc(myKeyboard);
    glutMouseFunc(myMouse);
    glutMotionFunc(myDraggedMouse);
}

void InitOpenGL()
{
    glClearColor(1.0,1.0,1.0,0.0);       // set white background color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
}
/* ********** ********** ********** */

int main (int argc, char **argv)
{
    glutInit(&argc, argv);
    CreateGlutWindow();
    CreateGlutCallbacks();
    InitOpenGL();

    glutMainLoop();

    return 0;
}
