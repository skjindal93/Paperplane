//
//  main.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 15/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"


double randf()	{
	return (double)rand() / (double)RAND_MAX;
}

struct Obstacle{
	GLfloat	z;
	Object *obj;
};

void iter(int);

/////////////////////////////////////////////////////////////
//////////////////////// Constants //////////////////////////
/////////////////////////////////////////////////////////////

GLint run, winW, winH, wLo, wHi, hLo, hHi, keyModifiers;
glm::vec3 eye, up, center;
GLfloat step, zoom;
GLUquadricObj* myQuadric = 0;
GLint slices, stacks;
GLfloat planeX, planeY, curZ;
Terrain *terr;
glm::vec3 specular, diffuse, ambient;
int t1, t2, frameCount, lastCount;
GLfloat fps;
GLvoid *font_style;
Object plane, star;
Material terrMaterial;
GLuint bgTex;
#define STAR_COUNT 7
glm::vec4 stars[STAR_COUNT];
#define OBJECT_COUNT 5
vector<Obstacle*> obstaclesList;
vector<Object> *allObjects;
GLfloat stargap, obstaclegap;
int score, xold, yold, vx, vy;
int maxX, maxY, hovered, tosave;
bool collide;
GLfloat planeFarZ;
int xpaused, ypaused;

void initConstants()	{
	run = 1, winW = 500, winH = 500, keyModifiers = 0;
	zoom = 1.0f;
	
	eye = glm::vec3(0.0f);
	up = glm::vec3(0.0f);
	up[1] = 1.0f;
	center = glm::vec3(0.0f);
	center[2] = -10.0f;
	
	step = 2.0f;
	slices = 150;
	stacks = 100;
	planeX = planeY = 0.0f;
	
	curZ = -5.0f;
	
	diffuse = glm::vec3(1.0f);
	specular = glm::vec3(240.0f, 240.0f, 188.0f)/250.0f;
	ambient = 1.0f - specular + 0.25f * diffuse;
	
	font_style = GLUT_BITMAP_HELVETICA_12;
	
	t1 = glutGet(GLUT_ELAPSED_TIME);
	frameCount = lastCount = 0;
	fps = 0.0f;
	
	terrMaterial.Ka = glm::vec3(0.7f);
	terrMaterial.Kd = glm::vec3(0.7f);
	terrMaterial.Ks = glm::vec3(0.8f);
	terrMaterial.s = 100.0f;
	
	stargap = 50.0f;
	maxX = 10.0f;
	maxY = 12.0f;
	
	obstaclegap = 120.0f;
	allObjects = readOBJ(string(PATH) + "objects.obj");
	for (int j = 0; j < OBJECT_COUNT; j++)	{
		int random = rand();
		Object *obj;
		obj = &((*allObjects)[random % allObjects->size()]);
		obj->timesUsed++;
		obj->load();

		Obstacle *obs;
		obs = new Obstacle();
		obs->z = -150.0f - j * obstaclegap;
		obs->obj = obj;
		obstaclesList.push_back(obs);
	}

	for(int i = 0; i < STAR_COUNT; ++i)
		stars[i] = glm::vec4(maxX * (randf() - 0.5), maxY * (randf() - 0.5), -40.0f - i * stargap, randf());

	tosave = 0;
	score = xold = yold = vx = vy = hovered = xpaused = ypaused = 0;
}

/////////////////////////////////////////////////////////////
////////////////////// GL Init, Resize //////////////////////
/////////////////////////////////////////////////////////////

// Initialize OpenGL's rendering modes
void GLInit()	{

	glEnable ( GL_DEPTH_TEST );
	glClear ( GL_ACCUM_BUFFER_BIT );
	myQuadric = gluNewQuadric();
	gluQuadricNormals( myQuadric, GL_TRUE );
	gluQuadricOrientation(myQuadric, GLU_OUTSIDE);
	gluQuadricDrawStyle(myQuadric, GLU_FILL);

	//	glPointSize(8);
	//	glLineWidth(5);
    
//	glEnable(GL_POINT_SMOOTH);
//	glEnable(GL_LINE_SMOOTH);
//	//	glEnable(GL_POLYGON_SMOOTH_HINT);
//	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    // Make round points, not square points
//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
//												//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
//												//	glEnable(GL_BLEND);
//												//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Clear the background of our window to white
	
	glEnable(GL_LIGHTING); //Enable lighting
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(glm::vec4(ambient, 1.0f)));
	
	glEnable(GL_LIGHT0); //Enable light #0
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(glm::vec4(specular, 1.0f)));
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  glm::value_ptr(glm::vec4(diffuse, 1.0f)));
	glLightfv(GL_LIGHT0, GL_AMBIENT,  glm::value_ptr(glm::vec4(ambient, 1.0f)));
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec4(0.0f, 20.0f, 1.0f, 0.0f)));
	
	glEnable(GL_LIGHT1); //Enable light #1
	glLightfv(GL_LIGHT1, GL_SPECULAR, glm::value_ptr(specular));
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  glm::value_ptr(diffuse));
	glLightfv(GL_LIGHT1, GL_AMBIENT,  glm::value_ptr(ambient));
	glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(glm::vec4(0.0f, 20.0f, -2000.0f, 0.0f)));
	
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glFrontFace(GL_CCW);
	
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading
	
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, glm::value_ptr(glm::vec4(glm::vec3(0.8f), 1.0f)));
	glFogf(GL_FOG_DENSITY, 0.0015f);
	
	cout << "Reading terrain..\n";
	
	Image *img = readP3(string(PATH) + "heightmap.desert.ppm");
	if(img != NULL)
		terr = new Terrain(img, string(PATH) + "colormap.desert.ppm", 10);
	cout << "Terrain loaded!\n";
	
	plane = (*readOBJ(string(PATH) + "plane.obj"))[0];
	plane.load();
	plane.save = true;
	
	star = (*readOBJ(string(PATH) + "star.obj"))[0];
	star.load();
	
	bgTex = loadTexture(string(PATH) + "bg.sky.ppm");
}

// glut's window resize function
// w, h - width and height of the window in pixels.
void resizeWindow(int w, int h)	{
	winW = w;
	winH = h;
	
	// Let's not change our view for different window sizes.
	int smaller = min(w,h);
	wLo = (w-smaller)/2;
	hLo = (h-smaller)/2;
	wHi = smaller + wLo;
	hHi = smaller + hLo;
	
	//Tell OpenGL how to convert from coordinates to pixel values
	glViewport(wLo, hLo, wHi - wLo, hHi - hLo);
	
	glMatrixMode(GL_PROJECTION); //Switch to setting the camera perspective
	
	//Set the camera perspective
	glLoadIdentity(); //Reset the camera
	gluPerspective(65.0f*zoom,            // FOV
				   1.0f, //(double)w / (double)h, // Aspect
				   0.5f,                // near-clipping
				   2000.0);               // far-clipping
	
}

void pause()	{
	run = 0;
	xpaused = xold;
	ypaused = yold;
}

void resume()	{
	run = 1;
	xold = xpaused;
	yold = ypaused;
	zoom = 1.0f;
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	zoom = 1.0f;
	resizeWindow(winW, winH);
	center[0] = center[1] = eye[0] = eye[1] = 0.0f;
	glutTimerFunc(1, iter, 0);
}




/////////////////////////////////////////////////////////////
/////////////// Keyboard and Mouse Functions ////////////////
/////////////////////////////////////////////////////////////

void keyboardFunc(unsigned char key, int x, int y)	{
	keyModifiers = glutGetModifiers();
	switch (key) {
		case 'r':
		case 'R':
			initConstants();
			resizeWindow(winW, winH);
			break;
		
		case ' ':
			run ? pause() : resume();
			break;
			
		case 27:                                    // "27" is theEscape key
			exit(0);
			
	}
}

void specialKeys(int k, int x, int y)	{
	keyModifiers = glutGetModifiers();
}

void hoverFunc(int x, int y)	{
	if(run)	{
		GLint w = wHi - wLo;
		GLint h = hHi - hLo;
		
		x -= wLo;
		y -= hLo;
		x = min(max(0, x), w);
		y = min(max(0, y), h);
		
		GLfloat dx = maxX *((float)x/w - 0.5f), dy = -maxY *((float)y/h - 0.5f);
		planeX = dx;
		planeY = dy;
		
		center[0] = eye[0] = dx/2.5f;
		center[1] = eye[1] = dy/2.5f;

		vx = 0.2 * (xold - x) + 0.8 * vx;
		vy = 0.2 * (yold - y) + 0.8 * vy;
	}
	hovered = 1;
	xold = x;
	yold = y;
}

void clickFunc(int button, int state, int x, int y)	{
	keyModifiers = glutGetModifiers();
}

void pan(int x, int y)	{
	float goRight = -180.0f*(float)(x - xold)/(float)winW, goUp = -180.0f*(float)(yold - y)/(float)winH;
	glm::vec3 dir = center - eye;
	GLfloat r = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
	dir = glm::normalize(dir);
	
	if((keyModifiers & GLUT_ACTIVE_SHIFT) > 0)	{
		// FOV Zoom
		float factor = pow(5, goRight/90 + goUp/90);
		zoom *= factor;
		resizeWindow(winW, winH);
	}
	else if((keyModifiers & GLUT_ACTIVE_ALT) > 0)	{
		// Take eye towards centre
		float factor = pow(5, goRight/90 + goUp/90);
		eye[0] = center[0] - r*factor*dir[0];
		eye[1] = center[1] - r*factor*dir[1];
		eye[2] = center[2] - r*factor*dir[2];
	}
	else	{
		glm::vec3 right = glm::cross(dir, up);
		right = glm::normalize(right);
		
		glm::vec3 d;
		d = -dir + up * sin(glm::radians(goUp)) + right * sin(glm::radians(goRight));
		d = glm::normalize(d);
		
		eye = center + r*d;
		
		// update up vector to be perpendicular to d
		glm::vec3 newUp = glm::cross(glm::cross(d, up), d);
		up = newUp;
		up = glm::normalize(up);
	}
	
	xold = x;
	yold = y;
}

void dragFunc(int x, int y)	{
	pan(x,y);
	glutPostRedisplay();
}




/////////////////////////////////////////////////////////////
/////////////////// Main Draw Functions /////////////////////
/////////////////////////////////////////////////////////////


void drawPlane()	{
	glPushMatrix();
	
	glTranslatef(planeX, planeY, -5.0f);
	glRotatef(vx * 1.5, 0.0f, 0.0f, 1.0f);
	glRotatef(10.0f, 1.0f, 0.0f, 0.0f);
	plane.render(0.5f, 0.3f, -0.9f);
	
	glPopMatrix();
}

void drawStar(float rotOffset = 0.0f)	{
	glPushMatrix();
	
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(360.0f*((double)((frameCount/2) % 50)/50.0f + rotOffset), 0.0f, 0.0f, 1.0f);
	glScalef(0.5f, 0.75f, 0.5f);
	star.render();
	
	glPopMatrix();
}

void drawObstacle(Obstacle *obs)	{
	glPushMatrix();
	
	glTranslatef(0.0f, 0.0f, obs->z);
	
	if(!strcmp(obs->obj->name, "Torus"))	{
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glScalef(1.0f, 1.5f, 1.5f);
		obs->obj->render(1.0f, true);
	} else if(!strcmp(obs->obj->name, "Cube"))	{
		glRotatef(45.0f, 1.0f, 1.0f, 0.0f);
		obs->obj->render(1.0f, true);
	}
	
	glPopMatrix();
}

void drawStatics()	{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	
	// Endless Terrain
	
	GLfloat size = 300.0f, height = 50.0f;
	int p = -curZ/size;
	glTranslatef(0.0f, -30.0f, -(size * p));
	
	// What fraction of terrain are we currently on
	float starting = (-curZ - size * p) / size;

	terrMaterial.apply();
	
	if(terr != NULL)
		terr->render(height, -size, starting, 4.0f);
	
	glPopAttrib();
	glPopMatrix();
	glPushMatrix();
	
	// Stars
	for(int i = 0; i < STAR_COUNT; ++i)	{
		if(stars[i][2] > curZ)	{
			stars[i][0] = maxX * (randf() - 0.5);
			stars[i][1] = maxY * (randf() - 0.5);
			stars[i][2] -= stargap * STAR_COUNT;
			stars[i][3] = randf();
		}
		if(stars[i][3] >= 0.0f)	{

			glTranslatef(stars[i][0], stars[i][1], stars[i][2]);
			drawStar(stars[i][3]);
			glTranslatef(-stars[i][0], -stars[i][1], -stars[i][2]);
			
			if(fabs(planeX - stars[i][0]) < 2.0f && fabs(planeY - stars[i][1]) < 2.0f && fabs(curZ - 5.0f - stars[i][2]) < 1.0f)	{
				score ++;
				stars[i][3] = -1.0f;
			}
		}
	}
	
	//Objects
	for(int j=0; j < OBJECT_COUNT; j++) {
		Obstacle *obs;
		obs = obstaclesList[j];
		
		if (obs->z > curZ) {
			if(! --obs->obj->timesUsed)
				obs->obj->unload();
			
			obs->z -= obstaclegap * OBJECT_COUNT;
			obs->obj = &( (*allObjects)[rand() % allObjects->size()] );
			obs->obj->timesUsed++;
			obs->obj->load();
			
			tosave ++;
			tosave %= OBJECT_COUNT;
		}
		else {
			if (tosave == j)
				obs->obj->save = true;
			
			drawObstacle(obs);
			
			if (obs->obj->save)
				collide = collide || plane.collision(obs->obj);

			delete[] obs->obj->modelView;
			obs->obj->modelView = NULL;

			obs->obj->save = false;
		}
	}
	
	glPopMatrix();
}

void drawMoving()	{
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
		
	// Background Texture
	
	glTranslatef(0.0f, 350.0f, -1000.0f);
	glScalef(700.0f, 700.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, bgTex);
	
	glBegin(GL_TRIANGLES);
	glNormal3f(0.0f, 0.0f, 1.0f);
	
	glTexCoord2f(1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	
	glTexCoord2f(0.0f, -1.0f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	
	glTexCoord2f(1.0f, -1.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
	glPopMatrix();
	glPopAttrib();
}

// glut's Main Display Function
void drawScene()	{
	drawPlane();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the rendering window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	collide = false;

//	UP vector rotation. TODO: Turn on?
//	glm::vec4 UP(0.0f, 1.0f, 0.0f, 1.0f);
//	glm::mat4x4 rot(1.0f);
//	rot = glm::rotate(rot, (float)pow(abs(vx), 0.6) * (vx < 0 ? 1.0f : -1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//	UP = rot * UP;
//	up[0] = UP[0];
//	up[1] = UP[1];
//	up[2] = UP[2];
	
	up = glm::normalize(up);
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
	// Only now draw anything you want to.
	
	//glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec3(0.0f, 50.0f, curZ-20.0f)));
	
	drawStatics();
	glTranslatef(0.0f, 0.0f, curZ);
	drawMoving();

//	glAccum(GL_MULT, 0.5);
//	glAccum(GL_ACCUM, 0.5);
//	glAccum(GL_RETURN, 1.0);
	
	// not blurring the plane
	drawPlane();
	
	// TODO: Uncomment the code below to blur plane partially.
//	GLfloat alpha = 0.2, beta = 0.5;
//	glAccum(GL_MULT, 1.0f - alpha);
//	glAccum(GL_ACCUM, alpha);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	
//	drawPlane();
//	glAccum(GL_ACCUM, beta * alpha);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	
//	glAccum(GL_RETURN, 1.0/(1.0 - beta));
//	drawPlane();
//	
//	// TODO: Now multiply the frame buffer by `beta` (or beta * alpha : check).
	
	if(collide)	{
		cout << "Hawwwwwww!\n";
		pause();
	}

	glutSwapBuffers();
	frameCount++;
	
	t2 = glutGet(GLUT_ELAPSED_TIME);
	double dt = ((double)t2 - (double)t1) / (double)1000;
	if(dt > 0.25)	{
		fps = (double)(frameCount - lastCount)/dt;
		cout << fps << " FPS, Score: " << score << "\n";
		t1 = glutGet(GLUT_ELAPSED_TIME);
		lastCount = frameCount;
	}
	
}

void iter(int dummy)	{
	if(run)	{
		curZ -= step;
		glutPostRedisplay();
	}
	
	eye[2] = curZ + 5.0f;
	center[2] = curZ - 5.0f;
	
	if(!hovered)	{
		vx *= 0.8;
		vy *= 0.8;
	}
	hovered = 0;
	
	if(run)
		glutTimerFunc(1, iter, 0);
}

int main(int argc, char * argv[])		{
	srand((unsigned)time(NULL));
	
	// Need to double buffer for animation
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_ACCUM | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	
	// Create and position the graphics window
	glutInitWindowPosition( 100, 100 );
	winW = winH = 500;
	glutInitWindowSize( winW, winH );
	glutCreateWindow( "PaperPlane 3D" );
	
	
	//initialize constants
	initConstants();

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

