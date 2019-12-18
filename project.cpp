#define _CRT_SECURE_NO_WARNINGS

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include "glew-2.1.0/include/GL/glew.h"
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glew.h>
#endif

#include <iostream>
#include <vector>
#include <fstream>

#include "vector.h"
#include "textfile.h"

using namespace std;

#ifdef __APPLE__
string path = "/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/";
#else
string path = "";
#endif

const int screenWidth = 1000;
const int screenHeight = 1000;

double angleX = 0, angleY = 0, angleZ = 0;

float* depth;

// Shader variables
GLuint vShader_01;
GLuint fShader_01;
GLuint glslProgram_color;
GLuint vShader_03;
GLuint fShader_03;
GLuint glslProgram_texture;

// Texture variables
unsigned char* image = nullptr;
GLuint texture[5];
int image_w;
int image_h;

float lPos[3] = { -6, 4, -17 };

// Earth variables
double earthR = 4.0;
double earthX = -3.0;
double earthY = 0.0;
double earthZ = 0.0;
bool selectEarth = true;

// Mars variables
double marsR = 2.7;
double marsX = 4.1;
double marsY = 0.0;
double marsZ = 0.0;
bool showMars = false;
bool selectMars = false;

// Flag variables
bool showFlag = false;
bool animateFlag = false;
double offset = 0.0;
int counter = 0;

bool spheresIntersection(Point c1, double r1, Point c2, double r2, Point& center, Vector& normal) {

    double length = (c1 - c2).norm();
    if (length > r1 + r2) return false;

    double diff = length - (r1 + r2);
    diff *= 0.1f;
    r1 += diff;
    double side1 = length - r1;
    double side2 = length - r2;
    double middle = length - side1 - side2;
    Vector tempVec = normalize(c1 - c2);
    Point tempPnt = c2 + (tempVec * ( side1 + middle / 2.0));
    normal = tempVec;
    center = tempPnt;

    cout << "plane center inside function = " << center << endl;
    cout << "plane normal inside function = " << normal << endl;
    return true;
}

void intersectionPlanePoints(Point center, Vector normal, Point &v1, Point &v2, Point &v3, Point &v4)
{
    double randomX = center.x + 8.0;
    double randomY = center.y + 2.5;

    if (normal.z == 0.0)
    {
        normal.z = 0.001;
    }
    double randomZ = ( (center.x - randomX) * (normal.x) + (center.y - randomY) * (normal.y) ) / (normal.z) + center.z;

    cout << "randomZ = " << randomZ << endl;

    Point p = Point(randomX, randomY, randomZ);

    // normalized tangent
    Vector t = normalize(p - center);
    // bi-tangent
    Vector b = t * normal;

    double D = earthR;

    v1 = center - t*D - b*D;
    v2 = center + t*D - b*D;
    v3 = center + t*D + b*D;
    v4 = center - t*D + b*D;

}
#define printOpenGLError() printOglError(__FILE__, __LINE__)
// Returns 1 if an OpenGL error occurred, 0 otherwise.
int printOglError(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

void printShaderInfoLog(GLuint obj)
{
    int infoLogLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0)
    {
        infoLog = (char *)malloc(infoLogLength);
        glGetShaderInfoLog(obj, infoLogLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infoLogLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength > 0)
    {
        infoLog = (char *)malloc(infoLogLength);
        glGetProgramInfoLog(obj, infoLogLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

GLuint setShaders(
        string vFile,
        string fFile,
        GLuint &vShader,
        GLuint &fShader)
{
    char *vs = nullptr;
    char *fs = nullptr;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    fShader = glCreateShader(GL_FRAGMENT_SHADER);

    vs = textFileRead(vFile.c_str());
    fs = textFileRead(fFile.c_str());

    const char * vv = vs;
    const char * ff = fs;

    glShaderSource(vShader, 1, &vv, nullptr);
    glShaderSource(fShader, 1, &ff, nullptr);

    free(vs); free(fs);

    glCompileShader(vShader);
    glCompileShader(fShader);

    printShaderInfoLog(vShader);
    printShaderInfoLog(fShader);

    GLuint pID = glCreateProgram();
    glAttachShader(pID, vShader);
    glAttachShader(pID, fShader);

    glLinkProgram(pID);
    printProgramInfoLog(pID);

    return pID;
}

// Read an 8 bit PPM file
unsigned char* readPPM(
        const char *filename,
        bool flag,
        int &dimx,
        int& dimy)
{
    FILE *ppmFile;
    char line[256];
    int i;
    int pixels;
    int x;
    int y;
    int r;
    int g;
    int b;
    unsigned char* p;
    unsigned char* f;

    if ((ppmFile = fopen(filename, "rb")) == nullptr)
    {
        printf("can't open %s\n", filename);
        exit(1);
    }

    fgets(line, 255, ppmFile);
    fgets(line, 255, ppmFile);
    while (line[0] == '#' || line[0] == '\n') fgets(line, 255, ppmFile);
    sscanf(line, "%d %d", &dimx, &dimy);
    fgets(line, 255, ppmFile);

    pixels = dimx * dimy;
    p = (unsigned char *)calloc(3 * pixels, sizeof(unsigned char));
    f = (unsigned char *)calloc(3 * pixels, sizeof(unsigned char));
    // 3 * pixels because of R, G and B channels
    i = 0;
    for (y = 0; y < dimy; y++)
    {
        for (x = 0; x < dimx; x++)
        {
            i = 3 * x + y * (3 * dimx);
            r = getc(ppmFile);
            p[i] = r;
            g = getc(ppmFile);
            p[i + 1] = g;
            b = getc(ppmFile);
            p[i + 2] = b;
        }
    }
    fclose(ppmFile);

    unsigned char *ptr1, *ptr2;

    ptr1 = p;
    ptr2 = f + 3 * dimx * (dimy - 1);
    for (y = 0; y < dimy; y++)
    {
        for (x = 0; x < dimx * 3; x++)
        {
            *ptr2 = *ptr1;
            ptr1++;
            ptr2++;
        }
        ptr2 -= (2 * 3 * dimx);
    }

    if (!flag)
    {
        free(p);
        p = 0;
        return(f);
    }
    else
    {
        free(f);
        f = 0;
        return(p);
    }
}

void myInit(void)
{
    glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
    glColor3f(0.0f, 0.0f, 0.0f);          // set the drawing color
    glPointSize(4.0);       // a �dot� is 4 by 4 pixels

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, (double) screenHeight / screenWidth, 0.01, 1000);

    glGenTextures(4, &texture[0]);

    cout << "Loading Earth.ppm" << endl;

    // Load and setup the texture 01
    string earthFilename = path + "Earth.ppm";
    image = readPPM(earthFilename.c_str(), false, image_w, image_h);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    if (image)
    {
        free(image);
    }
    image = 0;

    cout << "Loading Mars.ppm" << endl;
    // Load and setup the texture 05
    string marsFilename = path + "Mars.ppm";
    image = readPPM(marsFilename.c_str(), false, image_w, image_h);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    if (image)
    {
        free(image);
    }

    image = 0;

    cout << "Loading flag.ppm" << endl;
    // Load and setup the texture 05
    string flagFilename = path + "flag.ppm";
    image = readPPM(flagFilename.c_str(), false, image_w, image_h);

    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_w, image_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    if (image)
    {
        free(image);
    }

    image = 0;
}

void myDisplay(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glClearColor(1, 1, 1, 1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(glslProgram_color);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(lPos[0], lPos[1], lPos[2]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0, 0, 0);

    glUseProgram(glslProgram_texture);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(0, 0, -20);

    glRotated(angleX, 1, 0, 0);
    glRotated(angleY, 0, 1, 0);
    glRotated(angleZ, 0, 0, 1);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    GLuint texLoc = glGetUniformLocation(glslProgram_texture, "tex0");
    glUniform1i(texLoc, 0);

    texLoc = glGetUniformLocation(glslProgram_texture, "tex1");
    glUniform1i(texLoc, 1);

    texLoc = glGetUniformLocation(glslProgram_texture, "tex2");
    glUniform1i(texLoc, 2);

    texLoc = glGetUniformLocation(glslProgram_texture, "tex3");
    glUniform1i(texLoc, 3);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture[2]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture[3]);

    glColor3f(0, 0, 1);

    // Draw earth
    glPushMatrix ();
        glTranslatef(earthX, earthY, earthZ);
        GLUquadricObj* earth = nullptr;
        earth = gluNewQuadric();
        gluQuadricDrawStyle(earth, GLU_FILL);
        gluQuadricTexture(earth, true);
        gluQuadricNormals(earth, GLU_SMOOTH);
        gluSphere(earth, earthR, 80, 80);
        gluDeleteQuadric(earth);
    glPopMatrix ();

    GLuint loc = glGetUniformLocation(glslProgram_texture, "textureMars");
    glUniform1f(loc, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture[1]);

    // Draw Mars
    if (showMars)
    {
        glPushMatrix ();
        glTranslatef(marsX, marsY, marsZ);
        GLUquadricObj* mars = nullptr;
        mars = gluNewQuadric();
        gluQuadricDrawStyle(mars, GLU_FILL);
        gluQuadricTexture(mars, true);
        gluQuadricNormals(mars, GLU_SMOOTH);
        gluSphere(mars, marsR, 80, 80);
        gluDeleteQuadric(mars);
        glPopMatrix();

        if (showFlag)
        {
            Point p1 = Point(marsX + 1.0, marsY + 4.0, marsZ + 0);
            Point p2 = Point(marsX + 1.0, marsY + 6.0, marsZ + 0);
            Point p3 = Point(marsX + 4.0, marsY + 6.0, marsZ + 0);
            Point p4 = Point(marsX + 4.0, marsY + 4.0, marsZ + 0);

            // Draw flag handler
            loc = glGetUniformLocation(glslProgram_texture, "textureBlack");
            glUniform1f(loc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[3]);
            glPushMatrix ();
            glBegin(GL_LINE_STRIP);
            glLineWidth(20.0);
            glColor3d(0.0, 0.0, 0.0);
            glVertex3d(marsX + 1.0, marsY + 4.0, marsZ + 0.0);
            glVertex3d(marsX + 1.0, marsY, marsZ);
            glEnd();
            glPopMatrix ();

            loc = glGetUniformLocation(glslProgram_texture, "textureFlag");
            glUniform1f(loc, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[2]);

            // Draw flag
            glPushMatrix ();
            glBegin(GL_QUADS);
            glTexCoord2f(0, 0); glVertex3f(p1.x, p1.y, p1.z);
            glTexCoord2f(0, 1); glVertex3f(p2.x, p2.y, p2.z);
            glTexCoord2f(1, 1); glVertex3f(p3.x, p3.y, p3.z);
            glTexCoord2f(1, 0); glVertex3f(p4.x, p4.y, p4.z);
            glEnd();
            glPopMatrix ();
        }

        Point earthC = Point(earthX, earthY, earthZ);
        Point marsC = Point(marsX, marsY, marsZ);

        Point planeCenter;
        Vector planeNormal;

        bool spheresIntersect = spheresIntersection(earthC, earthR, marsC, marsR, planeCenter, planeNormal);

        cout << "spheresIntersect = " << spheresIntersect << endl;

        if (spheresIntersect)
        {
            cout << "plane center = " << planeCenter << endl;
            cout << "plane normal = " << planeNormal << endl;

            Point v1, v2, v3, v4;
            intersectionPlanePoints(planeCenter, planeNormal, v1, v2, v3, v4);

            cout << "v1 = " << v1 << endl;
            cout << "v2 = " << v2 << endl;
            cout << "v3 = " << v3 << endl;
            cout << "v4 = " << v4 << endl;

            glBegin(GL_QUADS);
            glLineWidth(20.0);
            glColor3d(0.0, 0.0, 0.0);
            glVertex3d(v1.x, v1.y, v1.z);
            glVertex3d(v2.x, v2.y, v2.z);
            glVertex3d(v3.x, v3.y, v3.z);
            glVertex3d(v4.x, v4.y, v4.z);
            glEnd();
        }
    }

    glutSwapBuffers();

    glFlush();
}

void myResize(int resizeWidth, int resizeHeight)
{
    glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
    if (key == 'w')
        angleX -= 5;

    else if (key == 's')
        angleX += 5;

    else if (key == 'a')
        angleY -= 5;

    else if (key == 'd')
        angleY += 5;

    else if (key == 'q')
        angleZ -= 5;

    else if (key == 'e')
        angleZ += 5;

    else if (key == '1')
    {
        lPos[0] -= 0.1f;
    }

    else if (key == '2')
    {
        lPos[0] += 0.1f;
    }

    else if (key == '3')
    {
        lPos[1] -= 0.1f;
    }

    else if (key == '4')
    {
        lPos[1] += 0.1f;
    }

    else if (key == '5')
    {
        lPos[2] -= 0.1f;
    }

    else if (key == '6')
    {
        lPos[2] += 0.1f;
    }

    else if (key == 'm')
    {
        showMars = !showMars;
    }

    else if (key == 'f')
    {
        showFlag = !showFlag;
    }

    else if (key == 'p')
    {
        animateFlag = !animateFlag;
    }

    // Select earth and disable mars
    else if (key == 'z')
    {
        selectEarth = true;
        selectMars = false;
    }

    // Select mars and disable earth
    else if (key == 'x')
    {
        selectMars = true;
        selectEarth = false;
    }

    else if (key == 'j')
    {
        if (selectEarth)
        {
            earthX -= 0.1f;
        }
        else if (selectMars)
        {
            marsX -= 0.1f;
        }
    }

    else if (key == 'l')
    {
        if (selectEarth)
        {
            earthX += 0.1f;
        }
        else if (selectMars)
        {
            marsX += 0.1f;
        }
    }

    else if (key == 'i')
    {
        if (selectEarth)
        {
            earthY += 0.1f;
        }
        else if (selectMars)
        {
            marsY += 0.1f;
        }
    }

    else if (key == 'k')
    {
        if (selectEarth)
        {
            earthY -= 0.1f;
        }
        else if (selectMars)
        {
            marsY -= 0.1f;
        }
    }

    else if (key == 'u')
    {
        if (selectEarth)
        {
            earthZ -= 0.1f;
        }
        else if (selectMars)
        {
            marsZ -= 0.1f;
        }
    }

    else if (key == 'o')
    {
        if (selectEarth)
        {
            earthZ += 0.1f;
        }
        else if (selectMars)
        {
            marsZ += 0.1f;
        }
    }

    glutPostRedisplay();
}

void myIdle()
{
    glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Group Project - Mohamad and Omar");

    glEnable(GL_DEPTH_TEST);

    glutDisplayFunc(myDisplay);

    glutReshapeFunc(myResize);
    glutKeyboardFunc(myKeyboard);
    glutIdleFunc(myIdle);
    glutMouseFunc(myMouse);

    myInit();

    glewInit();
    if (glewIsSupported("GL_VERSION_2_0"))
    {
        printf("Ready for OpenGL 2.0\n");
    }
    else
    {
        printf("OpenGL 2.0 not supported\n");
        exit(1);
    }

    glslProgram_color = setShaders(path + "color.vert", path + "color.frag", vShader_01, fShader_01);
    glslProgram_texture = setShaders(path + "texture.vert", path + "project.frag", vShader_03, fShader_03);

    glUseProgram(glslProgram_texture);

    glutMainLoop();

    return 0;
}
