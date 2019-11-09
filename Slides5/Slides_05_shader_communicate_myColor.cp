#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <windows.h> 
#include <math.h>
#include <GL/glew.h>
#include <gl/Gl.h>
#include <gl/glut.h>
#include <cmath>
#include <vector>

#include "vector.h"
#include "textfile.h"

using namespace std;

const int screenWidth = 640;
const int screenHeight = 640;

Point P1, P2;
int num_clicks = 0;

double angleX = 0, angleY = 0, angleZ = 0;

float* depth;

//Texture variables

unsigned char* image = 0;
GLuint		texture[2];
int image_w, image_h;

GLuint simpleVertexShader, simpleFragmentShader, simpleShaderProgram;
GLuint colorVertexShader, colorFragmentShader, colorShaderProgram;
GLuint flatVertexShader, flatFragmentShader, flatShaderProgram;

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char* file, int line)
{
	//
	// Returns 1 if an OpenGL error occurred, 0 otherwise.
	//
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
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}



GLuint setShaders(string vFile, string fFile, GLuint& vShader, GLuint& fShader) {

	char* vs = NULL, * fs = NULL;

	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead(vFile.c_str());
	fs = textFileRead(fFile.c_str());

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(vShader, 1, &vv, NULL);
	glShaderSource(fShader, 1, &ff, NULL);

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


unsigned char* readPPM(const char* filename, bool flag, int& dimx, int& dimy) {
	FILE* ppmfile;
	char          line[256];
	int           i, pixels, x, y, r, g, b;
	unsigned char* p;
	unsigned char* f;

	if ((ppmfile = fopen(filename, "rb")) == NULL) {
		printf("can't open %s\n", filename);
		exit(1);
	}

	fgets(line, 255, ppmfile);
	fgets(line, 255, ppmfile);
	while (line[0] == '#' || line[0] == '\n') fgets(line, 255, ppmfile);
	sscanf(line, "%d %d", &dimx, &dimy);
	fgets(line, 255, ppmfile);

	pixels = dimx * dimy;
	p = (unsigned char*)calloc(3 * pixels, sizeof(unsigned char));
	f = (unsigned char*)calloc(3 * pixels, sizeof(unsigned char));
	// 3 * pixels because of R, G and B channels
	i = 0;
	for (y = 0; y < dimy; y++) {
		for (x = 0; x < dimx; x++) {
			i = 3 * x + y * (3 * dimx);
			r = getc(ppmfile);
			p[i] = r;
			g = getc(ppmfile);
			p[i + 1] = g;
			b = getc(ppmfile);
			p[i + 2] = b;
		}
	}
	fclose(ppmfile);

	unsigned char* ptr1, * ptr2;

	ptr1 = p;
	ptr2 = f + 3 * dimx * (dimy - 1);
	for (y = 0; y < dimy; y++) {
		for (x = 0; x < dimx * 3; x++) {
			*ptr2 = *ptr1;
			ptr1++;
			ptr2++;
		}
		ptr2 -= (2 * 3 * dimx);
	}

	if (!flag) {
		free(p);
		p = 0;
		return(f);
	}
	else {
		free(f);
		f = 0;
		return(p);
	}

	return 0;
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

	depth = new float[screenWidth * screenHeight];
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

//<<<<<<<<<<<<<<<<<<<<<<< myInit >>>>>>>>>>>>>>>>>>>>
void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);       // set white background color
	glColor3f(0.0f, 0.0f, 0.0f);          // set the drawing color 
	glPointSize(4.0);       // a ‘dot’ is 4 by 4 pixels

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-4.0, 4.0, -4.0 * (double)screenHeight / screenWidth, 4.0 * (double)screenHeight / screenWidth, -4.0, 4.0);

	gluPerspective(45.0, (double)screenHeight / screenWidth, 0.01, 1000);
}
//<<<<<<<<<<<<<<<<<<<<<<<< myDisplay >>>>>>>>>>>>>>>>>
void myDisplay(void)
{
	//glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(0, 0, -20);

	glRotated(angleX, 1, 0, 0);
	glRotated(angleY, 0, 1, 0);
	glRotated(angleZ, 0, 0, 1);

	glUseProgram(simpleShaderProgram);

	GLuint loc = glGetUniformLocation(simpleShaderProgram, "myColor");
	glUniform1i(loc, 3);

	glutSolidTeapot(3);

	glEnd();
	
	glutSwapBuffers();

	glFlush();                 // send all output to display 
}

void myResize(int resizeWidth, int resizeHeight)
{
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (key == 'w')
		angleX += 5;

	if (key == 's')
		angleX -= 5;

	if (key == 'a')
		angleY += 5;

	if (key == 'd')
		angleY -= 5;

	if (key == 'q')
		angleZ -= 5;

	if (key == 'e')
		angleZ += 5;

	glutPostRedisplay();
}

void myIdle()
{

	glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (num_clicks == 0)
		{
			P1 = get_3D_pos(x, y);
			num_clicks++;
		}
		else if (num_clicks == 1)
		{
			P2 = get_3D_pos(x, y);
			num_clicks++;
		}
	}

}

//<<<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char** argv)
{
	glutInit(&argc, argv);          // initialize the toolkit
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE); // set display mode

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);



	glutInitWindowSize(screenWidth, screenHeight);     // set window size
	glutInitWindowPosition(100, 100); // set window position on screen
	glutCreateWindow("My Graphics App"); // open the screen window

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(myDisplay);     // register redraw function


	glutReshapeFunc(myResize);
	glutKeyboardFunc(myKeyboard);
	glutIdleFunc(myIdle);
	glutMouseFunc(myMouse);

	myInit();

	glewInit();

	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

	simpleShaderProgram = setShaders("simple.vert", "simple.frag", simpleVertexShader, simpleFragmentShader);
	colorShaderProgram = setShaders("color.vert", "color.frag", colorVertexShader, colorFragmentShader);
	flatShaderProgram = setShaders("flatten.vert", "flatten.frag", flatVertexShader, flatFragmentShader);

	glUseProgram(simpleShaderProgram);

	glutMainLoop();      // go into a perpetual loop

	return 0;
}
