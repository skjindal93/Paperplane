
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
using namespace std;

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "terrain.h"

/////////////////////////////////////////////////////////////
////////////////////// Render Functions /////////////////////
/////////////////////////////////////////////////////////////

//void closedCylinder(GLUquadric *quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks)	{
//	glPushMatrix();
//	gluCylinder(quad, base, top, height, slices, stacks);
//	gluDisk(quad, 0.0f, base, slices, 1);
//	glTranslatef(0.0f, 0.0f, height);
//	gluDisk(quad, 0.0f, top, slices, 1);
//	glPopMatrix();
//}



/////////////////////////////////////////////////////////////
//////////////////////// Constants //////////////////////////
/////////////////////////////////////////////////////////////

GLint run, winW, winH, keyModifiers;
glm::vec3 eye, up, center;
GLfloat step, zoom;
GLUquadricObj* myQuadric = 0;
GLint slices, stacks;
GLfloat planeX, planeY, curZ;
Terrain *terr;
char *terr_heightmap;

void initConstants()	{
	run = 1, winW = 500, winH = 500, keyModifiers = 0;
	zoom = 1.0f;
	
	eye = glm::vec3(0.0f);
	up = glm::vec3(0.0f);
	up[1] = 1.0f;
	center = glm::vec3(0.0f);
	center[2] = -10.0f;
	
	step = 0.5;
	slices = 150;
	stacks = 100;
	planeX = planeY = 0.0f;
	
	curZ = -5.0f;
	
	glutPostRedisplay();
}





/////////////////////////////////////////////////////////////
////////////////////// GL Init, Resize //////////////////////
/////////////////////////////////////////////////////////////

// Initialize OpenGL's rendering modes
void GLInit()	{
	
	glEnable ( GL_DEPTH_TEST );
	myQuadric = gluNewQuadric();
	gluQuadricNormals( myQuadric, GL_TRUE );
	gluQuadricOrientation(myQuadric, GLU_OUTSIDE);
	gluQuadricDrawStyle(myQuadric, GLU_FILL);
	
	//	glPointSize(8);
	//	glLineWidth(5);
    
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//	glEnable(GL_POLYGON_SMOOTH_HINT);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    // Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
												//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
												//	glEnable(GL_BLEND);
												//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	
	glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading
	
	Image *img = readP6(terr_heightmap);
	if(img != NULL)
		terr = new Terrain(img);
}

// glut's window resize function
// w, h - width and height of the window in pixels.
void resizeWindow(int w, int h)	{
	winW = w;
	winH = h;
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(65.0f*zoom,            // FOV
				   (double)w / (double)h, // Aspect
				   0.0001,                // near-clipping
				   2000.0);               // far-clipping
	
}





/////////////////////////////////////////////////////////////
/////////////// Keyboard and Mouse Functions ////////////////
/////////////////////////////////////////////////////////////

void keyboardFunc(unsigned char key, int x, int y)	{
	keyModifiers = glutGetModifiers();
	
//	glm::vec3 dir = glm::vec3(center[0] - eye[0], center[1] - eye[1], center[2] - eye[2]);
//	glm::normalize(dir);
//	glm::vec3 right = glm::cross(dir, up);
//	glm::normalize(right);
	
	glutPostRedisplay();
	
	switch (key) {
		case 'r':
		case 'R':
			initConstants();
			resizeWindow(winW, winH);
			break;
		
		case ' ':
			run ^= 1;
			break;
			
		case 27:                                    // "27" is theEscape key
			exit(0);
			
	}
}

void specialKeys(int k, int x, int y)	{
	keyModifiers = glutGetModifiers();
	glutPostRedisplay();
}

int xold, yold;

void hoverFunc(int x, int y)	{

	x += winW/6.0f;
	y += winH/6.0f;
	
	GLint w = 8.0f * winW / 6.0f;
	GLint h = 8.0f * winH / 6.0f;
	
	x = min(max(0, x), w);
	y = min(max(0, y), h);
	
	GLfloat dx = 10.0f*((float)x/w - 0.5f), dy = -10.0f*((float)y/h - 0.5f);
	planeX = dx;
	planeY = dy;
	
	center[0] = eye[0] = dx/2.5f;
	center[1] = eye[1] = dy/2.5f;
	
	// TODO: up vector.
	// When plane goes left or right, make up vector incline towards the center.
	
	xold = x;
	yold = y;
	glutPostRedisplay();
}

void clickFunc(int button, int state, int x, int y)	{
	keyModifiers = glutGetModifiers();
	glutPostRedisplay();
}

void dragFunc(int x, int y)	{
	glutPostRedisplay();
	//pan(x,y);
}




/////////////////////////////////////////////////////////////
/////////////////// Main Draw Functions /////////////////////
/////////////////////////////////////////////////////////////


void drawPlane()	{
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	
	GLfloat dimX = 2.0f, dimY = 1.0f, dimZ = -5.0f;

	dimX /= 2;
	dimY /= 2;
	
	glTranslatef(planeX, planeY, 0.0f);
	
	glBegin(GL_QUADS);
		glColor3f(0.1f, 0.2f, 0.7f);
		
		// Counter-clockwise order, normal pointing out
		
		// Back Face
		glVertex3f(-dimX, dimY, 0.0f);
		glVertex3f(-dimX, -dimY, 0.0f);
		glVertex3f(dimX, -dimY, 0.0f);
		glVertex3f(dimX, dimY, 0.0f);

		// Bottom Face
		glVertex3f(-dimX, -dimY, 0.0f);
		glVertex3f(-dimX, -dimY, dimZ);
		glVertex3f(dimX, -dimY, dimZ);
		glVertex3f(dimX, -dimY, 0.0f);
		
		// Left Face
		glVertex3f(-dimX, dimY, 0.0f);
		glVertex3f(-dimX, dimY, dimZ);
		glVertex3f(-dimX, -dimY, dimZ);
		glVertex3f(-dimX, -dimY, 0.0f);
		
		// Right Face
		glVertex3f(dimX, dimY, 0.0f);
		glVertex3f(dimX, -dimY, 0.0f);
		glVertex3f(dimX, -dimY, dimZ);
		glVertex3f(dimX, dimY, dimZ);
		
		// Front Face
		glVertex3f(-dimX, dimY, dimZ);
		glVertex3f(dimX, dimY, dimZ);
		glVertex3f(dimX, -dimY, dimZ);
		glVertex3f(-dimX, -dimY, dimZ);
		
		// Top Face
		glVertex3f(-dimX, dimY, 0.0f);
		glVertex3f(-dimX, dimY, dimZ);
		glVertex3f(dimX, dimY, dimZ);
		glVertex3f(dimX, dimY, 0.0f);
	
	glEnd();
	
	glPopAttrib();
	glPopMatrix();
}

void drawStatics()	{
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	
	GLfloat size = 500.0f, height = 25.0f;
	glTranslatef(0.0f, -50.0f, 0.0f);
	for(int i = 0, p = -curZ/size; i < 2; ++i, ++p)	{
		glTranslatef(0.0f, 0.0f, -(size * p + size / 2)/2);
		if(terr != NULL)
			terr->create(height, size);
	}
	
	glPopAttrib();
	glPopMatrix();
}

void drawMoving()	{
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	
	drawPlane();
	
	glPopAttrib();
	glPopMatrix();
}

// glut's Main Display Function
void drawScene()	{
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Clear the background of our window to white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the rendering window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	
	glm::normalize(up);
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	// Only now draw anything you want to.
	
	drawStatics();
	glTranslatef(0.0f, 0.0f, curZ);
	cout<<curZ<<endl;
	drawMoving();
	
	glutSwapBuffers();
	if(run)
		glutPostRedisplay();
	
}

void iter(int dummy)	{
	if(run)	{
		curZ -= step;
		glutPostRedisplay();
	}
	
	eye[2] = curZ + 5.0f;
	center[2] = curZ - 5.0f;
	
	glutTimerFunc(1, iter, 0);
}

int main(int argc, char * argv[])		{
	// Need to double buffer for animation
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	
	// Create and position the graphics window
	glutInitWindowPosition( 100, 100 );
	winW = winH = 500;
	glutInitWindowSize( winW, winH );
	glutCreateWindow( "PaperPlane 3D" );
	
	
	//initialize constants
	initConstants();
	
	terr_heightmap = new char[200];
	strcpy(terr_heightmap, "/Users/shivanker/Workplace/V Semester/Graphics/PaperPlane/PaperPlane/heightmap.world.5400x2700.ppm");
	
	// Initialize OpenGL.
	GLInit();
	
	// Callback for graphics image redrawing
	glutDisplayFunc( drawScene );
	
	// Set up callback functions for key presses
	glutKeyboardFunc( keyboardFunc );
    glutSpecialFunc(specialKeys);
	
	// Set up the callback function for resizing windows
	glutReshapeFunc( resizeWindow );
	
	// Click
	glutMouseFunc(clickFunc);
	// Drag
	glutMotionFunc(dragFunc);
	// Hover
	glutPassiveMotionFunc(hoverFunc);
	
	glutTimerFunc(0, iter, 0);
	
	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop(  );
    
	return 0;
}

