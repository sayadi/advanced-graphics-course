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

#include <cmath>

#include <vector>
#include "vector.h"

using namespace std;

const int screenWidth = 640;
const int screenHeight = 480;

/* ********** Global Variables ********** */

bool curve1Selected = true;
vector<Point> controlPoints1;
vector<Point> segmentPoints1;
vector<bool> points1Dragged;
Point curve1MidPoint;

bool curve2Selected = false;
vector<Point> controlPoints2;
vector<Point> segmentPoints2;
vector<bool> points2Dragged;
Point curve2MidPoint;

double subDivisions = 10;

// Purple
double activeControlPointRed = 1;
double activeControlPointGreen = 0;
double activeControlPointBlue = 1;

// Red
double inactiveControlPointRed = 1;
double inactiveControlPointGreen = 0;
double inactiveControlPointBlue = 0;

// Black
double activeSegmentRed = 0;
double activeSegmentGreen = 0;
double activeSegmentBlue = 0;

// Green
double inactiveSegmentRed = 0;
double inactiveSegmentGreen = 1;
double inactiveSegmentBlue = 0;

double segmentPointRed = 0;
double segmentPointGreen = 0;
double segmentPointBlue = 1;

bool showIntersections = false;

bool showMidPoints = false;

/* ********** ********** ********** */

/* ********** Utility functions ********** */

bool isPoint1AroundPoint2(double xOne, double yOne, double xTwo, double yTwo)
{
    return (abs(xOne - xTwo) <= 12) && (abs(yOne - yTwo) <= 12);
}

void setControlPointColor(bool isPointDragged, bool curveIsActive)
{
    int draggedPointRed = 0;
    int draggedPointGreen = 0;
    int draggedPointBlue = 0;

    if (isPointDragged)
    {
        glColor3d(draggedPointRed, draggedPointGreen, draggedPointBlue);
    }
    else
    {
        if (curveIsActive)
        {

            glColor3d(activeControlPointRed, activeControlPointGreen, activeControlPointBlue);
        }
        else
        {
            glColor3d(inactiveControlPointRed, inactiveControlPointGreen, inactiveControlPointBlue);
        }
    }
}

double binom(double n, double k)
{
    double coeff = 1;
    for (double i = n - k + 1; i <= n; i++)
    {
        coeff *= i;
    }
    for (double i = 1; i <= k; i++)
    {
        coeff /= i;
    }

    return coeff;
}

Point getSegmentPoint(vector<Point> controlPoints, double d)
{

    unsigned long order = controlPoints.size() - 1;

    double x = 0;
    double y = 0;

    for (unsigned long i = 0; i <= order; i++)
    {
        x = x + (binom(order, i) * pow((1 - d), (order - i)) * pow(d, i) * (controlPoints[i].x));
        y = y + (binom(order, i) * pow((1 - d), (order - i)) * pow(d, i) * (controlPoints[i].y));
    }

    return {x, y};
}

vector<Point> getSegmentPoints(vector<Point> controlPoints, int curveNo, Point &midPoint)
{
    vector<Point> *segmentPoints;
    if (curveNo == 1)
    {
        segmentPoints = &segmentPoints1;
    }
    else
    {
        segmentPoints = &segmentPoints2;
    }

    double t = 1.0 / subDivisions;
    for (double d = 0; d <= 1.01; d += t)
    {
        Point current = getSegmentPoint(controlPoints, d);
        segmentPoints->push_back(current);
    }

    midPoint = getSegmentPoint(controlPoints, 0.50);

    return *segmentPoints;
}

void drawCurve(vector<Point> controlPoints, int curveNo, Point &midPoint, bool curveIsActive)
{
    vector<Point> segmentPoints = getSegmentPoints(controlPoints, curveNo, midPoint);

    // Draw the curve's segments
    glLineWidth(2);
    if (curveIsActive)
    {
        glColor3d(activeSegmentRed, activeSegmentGreen, activeSegmentBlue);
    }
    else
    {
        glColor3d(inactiveSegmentRed, inactiveSegmentGreen, inactiveSegmentBlue);
    }

    glBegin(GL_LINE_STRIP);
    for (auto & point : segmentPoints)
    {
        glVertex2d(point.x, point.y);
    }
    glEnd();

    // Draw the segment points
    glPointSize(6);
    glColor3d(segmentPointRed, segmentPointGreen, segmentPointBlue);
    glBegin(GL_POINTS);
    for (int i = 0; i < segmentPoints.size(); i++)
    {
        glVertex2d(segmentPoints[i].x, segmentPoints[i].y);
    }
    glEnd();

}

void drawControlPoints(vector<Point> controlPoints, vector<bool> pointsDragged, bool curveIsActive)
{
    glPointSize(8);
    glBegin(GL_POINTS);
    for (int i = 0; i < controlPoints.size(); i++)
    {
        setControlPointColor(pointsDragged[i], curveIsActive);
        glVertex2d(controlPoints[i].x, controlPoints[i].y);
    }
    glEnd();
}

bool getIntersectionPoint(Point a, Point b, Point c, Point d, Point& i)
{
    Vector bVector = b - a;
    Vector dVector = d - c;
    Vector bPerp = perp(bVector);
    Vector dPerp = perp(dVector);
    Vector cVector = c - a;

    double t = (cVector % dPerp) / (bVector % dPerp);
    double u = -(cVector % bPerp) / (dVector % bPerp);

    i = a + t * bVector;

    return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}

vector<Point> getIntersectionPoints()
{
    vector<Point> intersectionPoints;

    for (int i = 0; i < segmentPoints1.size() - 1; i++)
    {
        Point a = segmentPoints1[i];
        Point b = segmentPoints1[i + 1];
        for (int j = 0; j < segmentPoints2.size() - 1; j++)
        {
            Point c = segmentPoints2[j];
            Point d = segmentPoints2[j + 1];
            Point i;
            if (getIntersectionPoint(a, b, c, d, i))
            {
                intersectionPoints.push_back(i);
            }
        }
    }

    return intersectionPoints;
}

void drawIntersectionPoints()
{
    vector<Point> intersectionPoints= getIntersectionPoints();

    glPointSize(8);
    // Orange
    glColor3d(1, 0.5, 0);
    glBegin(GL_POINTS);
    for (auto & intersectionPoint : intersectionPoints)
    {
        glVertex2d(intersectionPoint.x, intersectionPoint.y);
    }
    glEnd();
}

void drawMidPoints()
{
    glPointSize(8);
    // Dark purple
    glColor3d(0.5, 0.0, 0.5);
    glBegin(GL_POINTS);
    if (controlPoints1.size() >= 2)
    {
        glVertex2d(curve1MidPoint.x, curve1MidPoint.y);
    }
    if (controlPoints2.size() >= 2)
    {
        glVertex2d(curve2MidPoint.x, curve2MidPoint.y);
    }
    glEnd();
}

Point rotatePoint(Point p, Point c, bool clockwise, double angle = 0.0872665)
{
    double sineAngle = sin(angle);
    double cosineAngle = cos(angle);

    // Translate point back to origin:
    p.x -= c.x;
    p.y -= c.y;

    // Rotate point
    double xNew;
    double yNew;
    if (clockwise)
    {
        xNew = p.x * cosineAngle + p.y * sineAngle;
        yNew = -p.x * sineAngle + p.y * cosineAngle;
    }
    else
    {
        xNew = p.x * cosineAngle - p.y * sineAngle;
        yNew = p.x * sineAngle + p.y * cosineAngle;
    }

    // Translate point back:
    p.x = xNew + c.x;
    p.y = yNew + c.y;

    return p;
}

void rotateControlPoints(vector<Point> *controlPoints, Point midPoint, bool clockwise)
{
    for (int i = 0; i < controlPoints->size(); i++)
    {
        Point current = (*controlPoints)[i];
        (*controlPoints)[i] = rotatePoint(current, midPoint, clockwise);
    }
}

/* ********** ********** ********** */

/* ********** OpenGL functions ********** */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    segmentPoints1.clear();
    segmentPoints2.clear();

    segmentPointRed = 0;
    segmentPointGreen = 0;
    segmentPointBlue = 1;
    if (controlPoints1.size() >= 2)
    {
        drawCurve(controlPoints1, 1, curve1MidPoint, curve1Selected);
    }
    drawControlPoints(controlPoints1, points1Dragged, curve1Selected);

    segmentPointRed = 0;
    segmentPointGreen = 0;
    segmentPointBlue = 1;
    if (controlPoints2.size() >= 2)
    {
        drawCurve(controlPoints2, 2, curve2MidPoint, curve2Selected);
    }
    drawControlPoints(controlPoints2, points2Dragged, curve2Selected);

    if (showIntersections && controlPoints1.size() >= 2 && controlPoints2.size() >= 2)
    {
        drawIntersectionPoints();
    }

    if (showMidPoints)
    {
        drawMidPoints();
    }

    glutSwapBuffers();
}

void myDraggedMouse(int x, int y)
{

    y = screenHeight - y;

    vector<Point> *points = &controlPoints1;
    vector<bool> *pointsDragged = &points1Dragged;
    if (curve2Selected)
    {
        points = &controlPoints2;
        pointsDragged = &points2Dragged;
    }

    for (int i = 0; i < points->size(); i++)
    {
        if (isPoint1AroundPoint2(x, y, (*points)[i].x, (*points)[i].y))
        {
            (*points)[i] = Point(x, y);
            (*pointsDragged)[i] = true;
        }
    }

    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    y = screenHeight - y;

    vector<Point> *points = &controlPoints1;
    vector<bool> *pointsDragged = &points1Dragged;
    if (curve2Selected)
    {
        points = &controlPoints2;
        pointsDragged = &points2Dragged;
    }

    bool rightDown = button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN;

    if (rightDown)
    {
        points->push_back(Point(x, y));
        pointsDragged->push_back(false);
    }

    bool middleDown = button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN;
    if (middleDown)
    {
        if (!points->empty())
        {
            points->pop_back();
            pointsDragged->pop_back();
        }
    }

    bool leftUp = button == GLUT_LEFT_BUTTON && state == GLUT_UP;
    if (leftUp)
    {
        for (int i = 0; i < points->size(); i++)
        {
            (*pointsDragged)[i] = false;
        }
    }

    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
        case '1':
            curve1Selected = true;
            curve2Selected = false;
            break;

        case '2':
            curve1Selected = false;
            curve2Selected = true;
            break;

        case '6':
        {
            subDivisions = 5;
            break;
        }
        case '7':
        {
            subDivisions = 10;
            break;
        }
        case '8':
        {
            subDivisions = 20;
            break;
        }
        case '9':
        {
            subDivisions = 100;
            break;
        }

        case 'i':
        case 'I':
        {
            showIntersections = !showIntersections;
            break;
        }

        case 'm':
        case 'M':
        {
            showMidPoints = !showMidPoints;
            break;
        }

        case 'q':
        case 'Q':
        {
            if (curve1Selected)
            {
                rotateControlPoints(&controlPoints1, curve1MidPoint, false);
            }
            else
            {
                rotateControlPoints(&controlPoints2, curve2MidPoint, false);
            }
            break;
        }

        case 'e':
        case 'E':
        {
            if (curve1Selected)
            {
                rotateControlPoints(&controlPoints1, curve1MidPoint, true);
            }
            else
            {
                rotateControlPoints(&controlPoints2, curve2MidPoint, true);
            }
            break;
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
    glutCreateWindow ("Assignment 2 - Mohamad Al Sayadi");
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
