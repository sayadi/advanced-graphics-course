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

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>


using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

/* ********** Global Variables ********** */
int width = screenWidth;
int height = screenHeight;

int cosRed = 1;
int cosGreen = 0;
int cosBlue = 0;

int sinRed = 0;
int sinGreen = 1;
int sinBlue = 0;

GLenum drawMode = GL_LINE_STRIP;
double drawStep = 0.1;

GLfloat lineWidth = 2;
GLfloat pointSize = 4;

int rectRed = 0;
int rectGreen = 0;
int rectBlue = 1;

int pointsRed = 1;
int pointsGreen = 0;
int pointsBlue = 1;

int clicksNo = 0;
bool rectMode = false;
GLdouble xZero = 0; GLdouble yZero = 0;
GLdouble xOne = 0; GLdouble yOne = 0;
GLdouble xTwo = screenWidth; GLdouble yTwo = screenHeight;
GLdouble xMid; GLdouble yMid;

int draggedPointRed = 0;
int draggedPointGreen = 0;
int draggedPointBlue = 0;

bool isMidPointDragged = false;
bool isPoint1Dragged = false;
bool isPoint2Dragged = false;
/* ********** ********** ********** */

/* ********** Utility functions ********** */
bool isPoint1AroundPoint2(double xOne, double yOne, double xTwo, double yTwo)
{
    return (abs(xOne - xTwo) <= 12) && (abs(yOne - yTwo) <= 12);
}

void setPointColor(bool isPointDragged)
{
    if (isPointDragged)
    {
        glColor3d(draggedPointRed, draggedPointGreen, draggedPointBlue);
    }
    else
    {
        glColor3d(pointsRed, pointsGreen, pointsBlue);
    }
}

void drawFunction(string functionName)
{
    double (*func) (double) = nullptr;

    if (functionName == "cos")
    {
        glColor3f(cosRed, cosGreen, cosBlue);
        func = cos;
    }

    else if (functionName == "sin")
    {
        glColor3f(sinRed, sinGreen, sinBlue);
        func = sin;
    }

    double xIntervalStart = -6.28;
    double xIntervalEnd = 6.28;
    double xIntervalLenght = xIntervalEnd - xIntervalStart;

    double yIntervalStart = -1.2;
    double yIntervalEnd = 1.2;
    double yIntervalLength = yIntervalEnd - yIntervalStart;

    glBegin(drawMode);
    for (double x = xIntervalStart; x <= xIntervalEnd; x += drawStep)
    {
        double y = func(x);

        double sx = x * width  / xIntervalLenght + ( xZero + (width / 2));
        double sy = y * height / yIntervalLength + ( yZero + (height / 2));

        glVertex2d(sx, sy);
    }
    glEnd();
}
/* ********** ********** ********** */

/* ********** OpenGL functions ********** */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (rectMode)
    {
        // Draw the points
        glPointSize(4);

        glBegin(GL_POINTS);

        setPointColor(isPoint1Dragged);
        glVertex2d(xOne, yOne);

        setPointColor(isPoint2Dragged);
        glVertex2d(xTwo, yTwo);

        setPointColor(isMidPointDragged);
        glVertex2d(xMid, yMid);

        glEnd();

        // Draw the rectangle
        glColor3d(rectRed, rectGreen, rectBlue);

        glBegin(GL_LINE_LOOP);

        glVertex2d(xOne, yOne);
        glVertex2d(xTwo, yOne);
        glVertex2d(xTwo, yTwo);
        glVertex2d(xOne, yTwo);

        glEnd();

        width = abs(xOne - xTwo);
        height = abs(yOne - yTwo);

        xZero = min(xOne, xTwo);
        yZero = min(yOne, yTwo);
    }

    // Draw the functions
    glLineWidth(lineWidth);
    glPointSize(pointSize);

    drawFunction("cos");

    drawFunction("sin");

    glutSwapBuffers();
}

void myDraggedMouse(int x, int y)
{

    y = screenHeight - y;

    if (rectMode)
    {
        if (isPoint1AroundPoint2(x, y, xMid, yMid))
        {
            double delta_x = x - xMid;
            double delta_y = y - yMid;

            xMid = x;
            yMid = y;

            xOne += delta_x;
            xTwo += delta_x;

            yOne += delta_y;
            yTwo += delta_y;

            isMidPointDragged = true;
        }

        else if (isPoint1AroundPoint2(x, y, xOne, yOne))
        {
            xOne = x;
            yOne = y;

            xMid = (xOne + xTwo) / 2;
            yMid = (yOne + yTwo) / 2;

            isPoint1Dragged = true;
        }

        else if (isPoint1AroundPoint2(x, y, xTwo, yTwo))
        {
            xTwo = x;
            yTwo = y;

            xMid = (xOne + xTwo) / 2;
            yMid = (yOne + yTwo) / 2;

            isPoint2Dragged = true;
        }
    }

    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    y = screenHeight - y;

    bool leftDown = button == GLUT_LEFT_BUTTON && state == GLUT_DOWN;

    if (leftDown && clicksNo == 0)
    {
        cout << "Left button is down and click no is 0\n";

        xOne = x;
        yOne = y;
        clicksNo = 1;
    }

    else if (leftDown && clicksNo == 1)
    {
        cout << "Left button is down and click no is 1\n";

        xTwo = x;
        yTwo = y;

        xMid = (xOne + xTwo) / 2;
        yMid = (yOne + yTwo) / 2;

        clicksNo = 2;
        rectMode = true;
    }

    bool rightDown = button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN;
    if (rightDown)
    {
        clicksNo = 0;
        xZero = 0; yZero = 0;
        xOne = 0; yOne = 0;
        xTwo = screenWidth; yTwo = screenHeight;
        width = screenWidth;
        height = screenHeight;

        rectMode = false;
    }

    bool leftUp = button == GLUT_LEFT_BUTTON && state == GLUT_UP;
    if (leftUp)
    {
        isMidPointDragged = false;
        isPoint1Dragged = false;
        isPoint2Dragged = false;
    }

    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
    if (key == 'c')
    {
        // Flip the cos color
        cosRed = !cosRed;
        cosGreen = !cosGreen;

        // Flip the sin color
        sinRed = !sinRed;
        sinGreen = !sinGreen;
    }

    if (key == 'p')
    {
        if (drawMode == GL_LINE_STRIP)
        {
            drawMode = GL_POINTS;
        }

        else if (drawMode == GL_POINTS)
        {
            drawMode = GL_LINE_STRIP;
        }
    }

    if (key == 's')
    {
        if (drawStep == 0.1)
        {
            drawStep = 1.0;
        }
        else if (drawStep == 1.0)
        {
            drawStep = 0.1;
        }

    }

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
