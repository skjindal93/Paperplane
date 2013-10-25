//
//  main.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 15/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"


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
char *terr_heightmap, *terr_texture;
glm::vec3 specular, diffuse, ambient;
int t1, t2, frameCount;
GLfloat fps;
GLvoid *font_style;

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
	
	diffuse = glm::vec3(1.0f);
	specular = glm::vec3(240.0f, 240.0f, 188.0f)/250.0f;
	ambient = 1.0f - specular + 0.25f * diffuse;
	
	font_style = GLUT_BITMAP_HELVETICA_12;
	
	t1 = glutGet(GLUT_ELAPSED_TIME);
	frameCount = 0;
	fps = 0.0f;
	
	glutPostRedisplay();
}

//int _vscprintf (const char * format, va_list pargs) {
//	int retval;
//	va_list argcopy;
//	va_copy(argcopy, pargs);
//	retval = vsnprintf(NULL, 0, format, pargs);
//	va_end(argcopy);
//	return retval;
//}
//
////-------------------------------------------------------------------------
////  Draws a string at the specified coordinates.
////-------------------------------------------------------------------------
//void printw (float x, float y, float z, char* format, ...)
//{
//    va_list args;   //  Variable argument list
//    int len;        // String length
//    int i;          //  Iterator
//    char * text;    // Text
//	
//    //  Initialize a variable argument list
//    va_start(args, format);
//	
//    //  Return the number of characters in the string referenced the list of arguments.
//    // _vscprintf doesn't count terminating '\0' (that's why +1)
//    len = _vscprintf(format, args) + 1;
//	
//    //  Allocate memory for a string of the specified size
//    text = new char[len];
//	
//    //  Write formatted output using a pointer to the list of arguments
//	vsprintf(text, format, args);
//	//    vsprintf(text, len, format, args);
//	
//    //  End using variable argument list
//    va_end(args);
//	
//    //  Specify the raster position for pixel operations.
//    glRasterPos3f (x, y, z);
//	
//    //  Draw the characters one by one
//    for (i = 0; text[i] != '\0'; i++)
//		glutBitmapCharacter(font_style, text[i]);
//	
//    //  Free the allocated memory for the string
//	delete[] text;
//}


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
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(specular));
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  glm::value_ptr(diffuse));
	glLightfv(GL_LIGHT0, GL_AMBIENT,  glm::value_ptr(ambient));
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec3(0.0f, 20.0f, -10.0f)));
	
	glEnable(GL_LIGHT1); //Enable light #1
	glLightfv(GL_LIGHT1, GL_SPECULAR, glm::value_ptr(specular));
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  glm::value_ptr(diffuse));
	glLightfv(GL_LIGHT1, GL_AMBIENT,  glm::value_ptr(ambient));
	glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(glm::vec3(0.0f, 20.0f, -10.0f)));
		
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading
	
	Image *img = readP6(terr_heightmap);
	if(img != NULL)
		terr = new Terrain(img, terr_texture);
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear the background of our window to white
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the rendering window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	
	glm::normalize(up);
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	// Only now draw anything you want to.
	
	drawStatics();
	glTranslatef(0.0f, 0.0f, curZ);
	//cout<<curZ<<endl;
	drawMoving();
	
	glutSwapBuffers();
	frameCount++;
	
	t2 = glutGet(GLUT_ELAPSED_TIME);
	double dt = ((double)t2 - (double)t1) / (double)1000;
	if(dt > 0.25)	{
		fps = (double)frameCount/dt;
		cout << fps << " FPS\n";
		t1 = glutGet(GLUT_ELAPSED_TIME);
		frameCount = 0;
	}
	
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
	
#pragma GCC diagnostic ignored "-Wwrite-strings"
	terr_heightmap = "/Users/shivanker/Workplace/V Semester/Graphics/PaperPlane/PaperPlane/heightmap.world.540x270.ppm";
	terr_texture = "/Users/shivanker/Workplace/V Semester/Graphics/PaperPlane/PaperPlane/colormap.world.540x270.ppm";
	
	// Initialize OpenGL.
	GLInit();
	
	// Callback for graphics image redrawing
	glutDisplayFunc( drawScene );
	
	// Set up callback functions for key presses
	glutKeyboardFunc(keyboardFunc);
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

