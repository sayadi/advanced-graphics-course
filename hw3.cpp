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

const int screenWidth = 800;
const int screenHeight = 800;

double angleX = 0, angleY = 0, angleZ = 0;
double scaleF = 1.0;
double translateX = 0, translateY = 0, oldTx, oldTy;

float* depth;

// Shader variables
GLuint vShader_01;
GLuint fShader_01;
GLuint glslProgram_color;
GLuint vShader_03;
GLuint fShader_03;
GLuint glslProgram_texture;

//Texture variables
unsigned char* image = nullptr;
GLuint texture[4];
int image_w;
int image_h;

float lPos[3] = { -6, 4, -17 };
double triPos[3] = { -3.2, -1.2, -12.8 };

bool showClouds = false;
float offset = 0.0;
bool animateClouds = false;
bool bumpN = false;
bool highlight = false;
bool diffuseLight = false;
bool textureEarth = false;

bool drawLight = false;
bool drawRays = false;
bool drawTriangle = false;

bool IntersectTriangle(
        const Point& RayStart,
        const Vector& RayDir,
        const Point& T1,
        const Point& T2,
        const Point& T3,
        Point& I,
        Vector& R)
{
    //Implement the function. Find the intersection point with the plane.
    //Calculate the reflection vector only using the nearest intersection point.
    //Return true if the intersection point is inside the triangle.

    return false;
}

bool IntersectSphere(
        const Point& RayStart,
        const Vector& RayDir,
        const Point &Center,
        const double &Radius,
        Point& I1,
        Point& I2,
        Vector& R)
{
    //Implement the function. Find the two intersection points.
    //Calculate the reflection vector only using the nearest intersection point.
    //Return true if the intersections are found.

    return false;
}

void drawSphere(double radius, double r, double g, double b)
{
    glColor3d(r, g, b);

    GLUquadricObj* sphere = nullptr;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, true);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, radius, 80, 80);
    gluDeleteQuadric(sphere);
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

Point get_3D_pos(int x, int y)
{
    GLdouble pMatrix[16], mMatrix[16];

    glGetDoublev(GL_PROJECTION_MATRIX, pMatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX, mMatrix);

    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    y = viewport[3] - y;

    int winWidth = viewport[2];
    int winHeight = viewport[3];

    if (depth)
    {
        delete[] depth;
        depth = 0;
    }

    depth = new float[screenWidth*screenHeight];
    glReadPixels(0, 0, screenWidth, screenHeight, GL_DEPTH_COMPONENT, GL_FLOAT, depth);

    float z = depth[y * winWidth + x];

    cout << "Z = " << z << endl;

    if (z >= 1.0f - 1e-12 || z <= 1e-12)
        z = 0.5f;

    GLdouble wx = x;
    GLdouble wy = y;
    GLdouble wz = z;

    GLdouble px, py, pz;

    gluUnProject(wx, wy, wz, mMatrix, pMatrix, viewport, &px, &py, &pz);

    Point p = { (float)px, (float)py, (float)pz };

    cout << "Point = " << p << endl;

    return p;
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
    image = readPPM("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/Earth.ppm", false, image_w, image_h);

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

    cout << "Completed 25%" << endl;
    cout << "Loading Normals.ppm" << endl;

    // Load and setup the texture 02
    image = readPPM("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/Normals.ppm", false, image_w, image_h);

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

    cout << "Completed 50%" << endl;
    cout << "Loading Clouds.ppm" << endl;

    // Load and setup the texture 03
    image = readPPM("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/Clouds.ppm", false, image_w, image_h);

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

    cout << "Completed 75%" << endl;
    cout << "Loading Water.ppm" << endl;

    // Load and setup the texture 04
    image = readPPM("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/Water.ppm", false, image_w, image_h);

    glBindTexture(GL_TEXTURE_2D, texture[3]);
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

    cout << "Completed 100%" << endl;
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
    if (drawLight)
    {
        drawSphere(0.2, 1, 1, 0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0, 0, 0);

    Point LightPos = Point(lPos[0], lPos[1], lPos[2]);
    Point Surface1 = Point(0, 2, -18);
    Point Surface2 = Point(0, -2, -17);

    Vector CL = Surface1 - LightPos;
    Vector CS = Surface2 - LightPos;

    CL = normalize(CL);
    CS = normalize(CS);

    Point EndCL = LightPos + 16 * CL;
    Point EndCS = LightPos + 16 * CS;

    if (drawRays)
    {
        glBegin(GL_LINES);
        glVertex3d(LightPos.x, LightPos.y, LightPos.z);
        glVertex3d(EndCL.x, EndCL.y, EndCL.z);
        glEnd();

        glBegin(GL_LINES);
        glVertex3d(LightPos.x, LightPos.y, LightPos.z);
        glVertex3d(EndCS.x, EndCS.y, EndCS.z);
        glEnd();
    }

    Point I1;
    Point I2;
    Vector R;
    bool f = IntersectSphere(
            LightPos,
            CL,
           Point(0, 0, -20),
           4,
           I1,
           I2,
           R);

    if (drawRays && f)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslated(I1.x, I1.y, I1.z);
        drawSphere(0.1, 1, 0, 0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslated(I2.x, I2.y, I2.z);
        drawSphere(0.1, 1, 0, 0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        Point REnd = I1 + 10 * R;

        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
        glVertex3d(I1.x, I1.y, I1.z);
        glVertex3d(REnd.x, REnd.y, REnd.z);
        glEnd();

        f = IntersectSphere(
                LightPos,
                CS,
                Point(0, 0, -20),
                4,
                I1,
                I2,
                R);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslated(I1.x, I1.y, I1.z);
        drawSphere(0.1, 1, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glTranslated(I2.x, I2.y, I2.z);
        drawSphere(0.1, 1, 0, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        REnd = I1 + 10 * R;

        glColor3f(1, 0, 0);

        Point T1 = Point(triPos[0] - 2, triPos[1] + 2, triPos[2] - 2);
        Point T2 = Point(triPos[0], triPos[1] - 2, triPos[2]);
        Point T3 = Point(triPos[0] + 2, triPos[1] - 2, triPos[2] + 2);

        bool didTriangleIntersect;
        Point tI;
        Vector tR;

        if (drawTriangle)
        {
            glBegin(GL_TRIANGLES);
            glNormal3d(0, 1, 0);
            glVertex3d(T1.x, T1.y, T1.z);
            glVertex3d(T2.x, T2.y, T2.z);
            glVertex3d(T3.x, T3.y, T3.z);
            glEnd();

            didTriangleIntersect = IntersectTriangle(
                    I1,
                    R,
                    T1,
                    T2,
                    T3,
                    tI,
                    tR);

            if (didTriangleIntersect)
            {
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                glTranslated(tI.x, tI.y, tI.z);
                drawSphere(0.1, 1, 0, 1);

                REnd = tI;
            }
        }

        glColor3f(0, 0, 0);
        glBegin(GL_LINES);
        glVertex3d(I1.x, I1.y, I1.z);
        glVertex3d(REnd.x, REnd.y, REnd.z);
        glEnd();

        if (drawTriangle && didTriangleIntersect)
        {
            Point tREnd = tI + 10 * tR;
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();

            glColor3f(0, 0, 0);
            glBegin(GL_LINES);
            glVertex3d(tI.x, tI.y, tI.z);
            glVertex3d(tREnd.x, tREnd.y, tREnd.z);
            glEnd();
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

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

    GLuint loc = glGetUniformLocation(glslProgram_texture, "lPos");
    glUniform3f(loc, lPos[0], lPos[1], lPos[2]);

    loc = glGetUniformLocation(glslProgram_texture, "showClouds");
    glUniform1i(loc, showClouds);

    if (animateClouds)
    {
        offset+=0.0001;
        if (offset > 1)
        {
            offset = 0.0001;
        }
    }
    loc = glGetUniformLocation(glslProgram_texture, "offset");
    glUniform1f(loc, offset);

    loc = glGetUniformLocation(glslProgram_texture, "bumpN");
    glUniform1f(loc, bumpN);

    loc = glGetUniformLocation(glslProgram_texture, "highlight");
    glUniform1f(loc, highlight);

    loc = glGetUniformLocation(glslProgram_texture, "diffuseLight");
    glUniform1f(loc, diffuseLight);

    loc = glGetUniformLocation(glslProgram_texture, "textureEarth");
    glUniform1f(loc, textureEarth);

    glColor3f(0, 0, 1);

    GLUquadricObj* sphere = nullptr;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricTexture(sphere, true);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluSphere(sphere, 4.0, 80, 80);
    gluDeleteQuadric(sphere);

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

    else if (key == 'n')
    {
        animateClouds = !animateClouds;
    }

    else if (key == 'b')
    {
        bumpN = !bumpN;
    }

    else if (key == 'h')
    {
        highlight = !highlight;
    }

    else if (key == 'f')
    {
        diffuseLight = !diffuseLight;
    }

    else if (key == 'l')
    {
        drawLight = !drawLight;

        if (!drawLight)
        {
            drawRays = false;
        }
    }

    else if (key == 'r' && drawLight)
    {
        drawRays = !drawRays;
        if (!drawRays)
            drawTriangle = false;
    }

    else if (key == 'g')
    {
        drawTriangle = !drawTriangle;

        if (!drawLight || !drawRays)
            drawTriangle = false;
    }


    else if (key == 't')
    {
        textureEarth = !textureEarth;
    }

    else if (key == 'c')
    {
        showClouds = !showClouds;
    }

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
    //glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE); // set display mode

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    glutInitWindowSize(screenWidth, screenHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Mohamed Al Sayadi - Assignment 3");

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

    glslProgram_color = setShaders("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/color.vert", "/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/color.frag", vShader_01, fShader_01);
    glslProgram_texture = setShaders("/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/texture.vert", "/Users/msayadi/Fall-2019/Advanced_Computer_Graphics/advanced-graphics-course/hw3.frag", vShader_03, fShader_03);

    glUseProgram(glslProgram_texture);

    glutMainLoop();

    return 0;
}
