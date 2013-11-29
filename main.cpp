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
	GLfloat	x, y, z;
	ObjectGroup *obj;
};

bool zcomparator(glm::vec4 a, glm::vec4 b)	{
	return a.z > b.z;
}

void iter(int);
void genDepthFBO(GLuint*, GLuint*, int, int);
void genFBO(GLuint*, GLuint*, GLuint*);

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
ObjectGroup plane, star;
Material terrMaterial;
GLuint bgTex;
#define STAR_COUNT 7
vector<glm::vec4> stars(STAR_COUNT);
#define OBJECT_COUNT 5
vector<Obstacle*> obstaclesList;
vector<ObjectGroup> allObjects;
GLfloat stargap, obstaclegap;
int score, xold, yold, vx, vy;
int maxX, maxY, hovered, tosave;
bool collide;
GLfloat planeFarZ;
int xpaused, ypaused;
vector<Shader> shaders;
bool drawless, boost;
#define SHADOW_MAP_RATIO 3
glm::vec3 lightpos, lightdir;
glm::mat4 lightProj, lightView, cameraProj, cameraView;
GLuint shadowmap, shadowFBO;
GLuint stars_tex, fbo, depthbuf;
GLuint environment;
GLfloat starsW, starsH, shadowW, shadowH;
al openal;
GLfloat collisionFrame;
bool ended;

float shake(){
    float time = frameCount - collisionFrame;
    float sinCollision = 4.0f * sin (time / 200.0f);
    float sin2Collision = 1.5f * sin (time / 1.0f);
    float expCollision = exp2f(-time/25.0f) * sin2Collision + sinCollision;
    return eye[1] + expCollision;
}

void preGLInit()	{
	ended = false;
	run = 1, winW = 500, winH = 500, keyModifiers = 0;
	zoom = 1.0f;
	
	eye = glm::vec3(0.0f);
	up = glm::vec3(0.0f);
	up[1] = 1.0f;
	center = glm::vec3(0.0f);
	center[2] = -100.0f;
	
	step = 2.0f;
	slices = 150;
	stacks = 100;
	planeX = planeY = 0.0f;
	
	curZ = -5.0f;
	
	diffuse = glm::vec3(1.0f);
	specular = glm::vec3(180.0f, 180.0f, 128.0f)/255.0f;
	ambient = 1.0f - specular + 0.25f * diffuse;
	
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

	for(int i = 0; i < STAR_COUNT; ++i)
		stars[i] = glm::vec4(maxX * (randf() - 0.5), maxY * (randf() - 0.5), -40.0f - i * stargap, randf());

	tosave = 0;
	score = xold = yold = vx = vy = hovered = xpaused = ypaused = 0;
	
	drawless = false;
	lightpos = glm::vec3(-25.0f, 75.0f, 120.0f);
	lightdir = -glm::vec3(25.0f, -75.0f, -160.0f);

	boost = false;

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
    
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH_HINT);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);    // Make round points, not square points
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);     // Antialias the lines
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // Clear the background of our window to transparent white (helps with glow)
	
	glEnable(GL_LIGHTING); //Enable lighting
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(glm::vec4(ambient, 1.0f)));
	
	glEnable(GL_LIGHT0); //Enable light #0
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(glm::vec4(specular, 1.0f)));
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  glm::value_ptr(glm::vec4(diffuse, 1.0f)));
	glLightfv(GL_LIGHT0, GL_AMBIENT,  glm::value_ptr(glm::vec4(ambient, 1.0f)));
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec4(lightdir, 0.0f)));
	
	glEnable(GL_LIGHT1); //Enable light #1
	glLightfv(GL_LIGHT1, GL_SPECULAR, glm::value_ptr(glm::vec4(0.0f)));
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  glm::value_ptr(diffuse));
	glLightfv(GL_LIGHT1, GL_AMBIENT,  glm::value_ptr(glm::vec4(0.0f)));
	glLightfv(GL_LIGHT1, GL_POSITION, glm::value_ptr(glm::vec4(0.0f, 200.0f, -2000.0f, 0.0f)));

//	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	//glFrontFace(GL_CCW);
	
    glEnable(GL_NORMALIZE); //Have OpenGL automatically normalize our normals
    glShadeModel(GL_SMOOTH); //Enable smooth shading
	
	glEnable(GL_FOG);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, glm::value_ptr(glm::vec4(glm::vec3(0.8f), 1.0f)));
	glFogf(GL_FOG_DENSITY, 0.0015f);

	glActiveTexture(GL_TEXTURE0);
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
	glMatrixMode(GL_MODELVIEW);

}

void postGLInit()	{
	// Initialising the shader with the file names and then pushing into the vector somehow doesn't work. :-/
#ifndef __APPLE__
	glewInit();
#endif
	for(int i = 0; i < 1; ++i)	{
		Shader s;
		shaders.push_back(s);
	}
#ifdef __APPLE__
	shaders[0].init(string(PATH) + "tex.vs", string(PATH) + "glowtex.apple.fs");
#else
	shaders[0].init(string(PATH) + "tex.vs", string(PATH) + "glowtex.fs");
#endif
	
	Image *img = readP3(string(PATH) + "heightmap.desert.ppm");
	if(img != NULL)
		terr = new Terrain(img, string(PATH) + "colormap.desert.ppm", 10);
	
	plane.readOBJ(string(PATH) + "plane.obj");
	plane.load();
	plane.save = true;
	
	star.readOBJ(string(PATH) + "star.obj");
	star.load();
	
	allObjects.clear();
	allObjects.resize(3);
	allObjects[0].readOBJ(string(PATH) + "torus.obj");
	allObjects[1].readOBJ(string(PATH) + "cube.obj");
	allObjects[2].readOBJ(string(PATH) + "house.obj");
	
	for (int j = 0; j < OBJECT_COUNT; j++)	{
		int random = rand();
		ObjectGroup *obj;
		obj = &(allObjects[random % allObjects.size()]);
		obj->timesUsed++;
		obj->load();
		
		Obstacle *obs;
		obs = new Obstacle();
		obs->z = -150.0f - j * obstaclegap;
		obs->x = (randf() - 0.5) * 1.5 * maxX;
		obs->y = (randf() - 0.5) * 1.5 * maxY;
		obs->obj = obj;
		obstaclesList.push_back(obs);
	}

	
	environment = loadTexture(string(PATH) + "environment.ppm");
	
	//Load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	//Depth states
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	
	//Use the color as the ambient and diffuse material
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	
	//White specular material color, shininess 16
	glMaterialfv(GL_FRONT, GL_SPECULAR, glm::value_ptr(glm::vec4(1,1,1,1)));
	glMaterialf(GL_FRONT, GL_SHININESS, 16.0f);
	
	starsW = winW;
	starsH = winH;
	genFBO(&stars_tex, &fbo, &depthbuf);
	shadowW = winW * SHADOW_MAP_RATIO;
	shadowH = winH * SHADOW_MAP_RATIO;
	genDepthFBO(&shadowFBO, &shadowmap, shadowW, shadowH);
}

void pause()	{
	run = 0;
	xpaused = xold;
	ypaused = yold;
}

void resume(int single = 0)	{
	ended = false;
	run = single ^ 1;
	xold = xpaused;
	yold = ypaused;
	zoom = 1.0f;
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	zoom = 1.0f;
	resizeWindow(winW, winH);
	center[0] = center[1] = eye[0] = eye[1] = 0.0f;
    step = 2.0f;
    collide = false;
	glutTimerFunc(1, iter, single);
}




/////////////////////////////////////////////////////////////
/////////////// Keyboard and Mouse Functions ////////////////
/////////////////////////////////////////////////////////////

void keyboardFunc(unsigned char key, int x, int y)	{
	keyModifiers = glutGetModifiers();
	switch (key) {
		
		case ' ':
			if(ended)
				resume();
			else
				run ? pause() : resume();
			break;

		case 13:
            if (!ended)
				run ? pause() : resume(1);
			break;
			
		case 'n':
		case 'N':
            if (!ended)
				boost = !boost;
			break;
			
		case 27:                                    // "27" is theEscape key
			exit(0);
			
	}
}

void specialKeys(int k, int x, int y)	{
	keyModifiers = glutGetModifiers();
}

void hoverFunc(int x, int y)	{
    if (ended)
        return;
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
		
		center[0] = eye[0] = dx/1.5f;
		center[1] = eye[1] = dy/1.5f;

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
    if (!ended){
		pan(x,y);
		glutPostRedisplay();
    }
}




/////////////////////////////////////////////////////////////
/////////////////// Main Draw Functions /////////////////////
/////////////////////////////////////////////////////////////

void setFrameBuffer(GLuint fbuf)	{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbuf);
	resizeWindow(winW, winH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the rendering window
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
	
	up = glm::normalize(up);
    if (ended)
        gluLookAt(eye[0],shake(), eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
    else
        gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
}

// generate an FBO
void genFBO(GLuint *color_tex, GLuint *fb, GLuint *depth_rb)	{
	glGenTextures(1, color_tex);
	glBindTexture(GL_TEXTURE_2D, *color_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	//NULL means reserve texture memory, but texels are undefined
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, winW, winH, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	
	glGenFramebuffers(1, fb);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, *fb);
	
	//Attach 2D texture to this FBO
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, *color_tex, 0);
	
	glGenRenderbuffersEXT(1, depth_rb);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, *depth_rb);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, winW, winH);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, *depth_rb);
	
	GLenum status;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(status != GL_FRAMEBUFFER_COMPLETE_EXT)	{
		cerr << "genFBO failed!" << endl;
		glDeleteTextures(1, color_tex);
		*color_tex = 0;
		glDeleteFramebuffers(1, fb);
		*fb = 0;
		glDeleteRenderbuffersEXT(1, depth_rb);
		*depth_rb = 0;
	}
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void genDepthFBO(GLuint *fb, GLuint *depth, int w, int h)	{
	
	//Create the shadow map texture
	glGenTextures(1, depth);
	glBindTexture(GL_TEXTURE_2D, *depth);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(0)));
	
	// Create a FBO
	glGenFramebuffers(1, fb);
	
	// Bind the frame buffer to the curent context and tell openGL that
	// the FBO won't be writing any color information and bind the texture ID
	// to the texture component of the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, *fb);
	
	// Instruct openGL that we won't bind a color texture with the currently binded FBO
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	// Set the texture to be at the depth attachment point of the FBO
	glFramebufferTexture2D(
						   GL_FRAMEBUFFER,
						   GL_DEPTH_ATTACHMENT,
						   GL_TEXTURE_2D,
						   *depth,
						   0);
	
	// check FBO status
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE )	{
		cerr << "genDepthFBO failed!" << endl;
		glDeleteFramebuffers(1, fb);
		*fb = 0;
		glDeleteTextures(1, depth);
		*depth = 0;

	}
	
	// switch back to window-system-provided framebuffer
	// and unbind the texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderSkybox(glm::vec3 position,GLuint environment)
{
	// Save Current Matrix
	glPushMatrix();
	glPushAttrib(GL_ENABLE_BIT);
	
    glColor4f(1.0, 1.0, 1.0,1.0f);
 
	// Second Move the render space to the correct position (Translate)
    glTranslatef(position.x,position.y,position.z);

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,environment);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
	
	gluSphere(myQuadric,500,100,100);
    
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

	// Load Saved Matrix
	glPopAttrib();
	glPopMatrix();
 
}

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
    if(ended)
     glRotatef(360.0f*((double)(((int)(collisionFrame+0.5)/2) % 50)/50.0f + rotOffset), 0.0f, 0.0f, 1.0f);
        else
	glRotatef(360.0f*((double)((frameCount/2) % 50)/50.0f + rotOffset), 0.0f, 0.0f, 1.0f);
	glScalef(0.5f, 0.75f, 0.5f);
	
	star.render();
		
	glPopMatrix();
}

void drawStars()	{
	glPushMatrix();
		
	if(stars[0][2] > eye[2])	{
		stars[0].x = maxX * (randf() - 0.5);
		stars[0].y = maxY * (randf() - 0.5);
		stars[0].z -= stargap * STAR_COUNT;
		stars[0][3] = randf();
	}
	
	sort(stars.begin(), stars.end(), zcomparator);
	
	if(fabs(planeX - stars[0].x) < 2.0f && fabs(planeY - stars[0].y) < 2.0f && fabs(curZ - 5.0f - stars[0].z) < 1.0f && stars[0][3] >= 0.0f)	{
		score ++;
		newExplosion(stars[0].x, stars[0].y, stars[0].z);
		openal.play(1);
		stars[0][3] = -1.0f;
	}
	
	if(starsW != winW || starsH != winH)	{
		glDeleteTextures(1, &stars_tex);
		glDeleteFramebuffers(1, &fbo);
		glDeleteRenderbuffersEXT(1, &depthbuf);
		
		starsW = winW;
		starsH = winH;
		genFBO(&stars_tex, &fbo, &depthbuf);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}

	
	for(int i = STAR_COUNT - 1; i >= 0; i--)	{
		if(stars[i][3] >= 0.0f)	{
			
			glClearColor(0.7, 0.7, 0.2, 0.0);
			setFrameBuffer(fbo);
			
			glPushMatrix();
			glTranslatef(stars[i][0], stars[i][1], stars[i][2]);
			drawStar(stars[i][3]);
			glPopMatrix();
			
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, stars_tex);
			
			shaders[0].bind();
			GLuint fragtex = glGetUniformLocation(shaders[0].pID, "tex");
			GLint w = glGetUniformLocation(shaders[0].pID, "w");
			GLint h = glGetUniformLocation(shaders[0].pID, "h");
			glUniform1i(fragtex, 0);
			glUniform1i(w, winW);
			glUniform1i(h, winH);
			
			GLfloat scale = 2.0;
			glPushMatrix();
			glTranslatef(stars[i][0], stars[i][1], stars[i][2]);
			glScalef(scale, scale, scale);
			drawStar(stars[i][3]);
			glPopMatrix();

			shaders[0].unbind();
		}
	}

//	glDeleteTextures(1, &stars_tex);
//	glDeleteFramebuffers(1, &fbo);
//	glDeleteRenderbuffersEXT(1, &depthbuf);
	glPopMatrix();
}

void drawObstacle(Obstacle *obs)	{
	glPushMatrix();
	
	glTranslatef(0.0f, 0.0f, obs->z);
	
	if(!obs->obj->name.compare("Torus"))	{
		glTranslatef(obs->x, 0.0f, 0.0f);
		glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
		glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
		glScalef(1.0f, 1.5f, 1.5f);
		obs->obj->render(1.0f, true);
	} else if(!obs->obj->name.compare("Cube"))	{
		glTranslatef(obs->x, obs->y, 0.0f);
		glRotatef(45.0f, 1.0f, 1.0f, 0.0f);
		obs->obj->render(1.0f, true);
	} else if(!obs->obj->name.compare("House"))	{
		glTranslatef(obs->x, 0.0f, 0.0f);
		glScalef(1.2, 1.2, 1.2);
		glTranslatef(-5.0, -10.0, 0.0);
		glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
		obs->obj->render(2.0f, true);
	}
	
	glPopMatrix();
}

void drawTerrain()	{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	
	// Endless Terrain
	GLfloat size = 300.0f, height = 50.0f;
	int p = -curZ/size;
	glTranslatef(0.0f, -20.0f, -(size * p));
	
	// What fraction of terrain are we currently on
	float starting = (-curZ - size * p) / size;

	if(!drawless)
		terrMaterial.apply();
	
	if(terr != NULL)
		terr->render(height, -size, starting, 4.0f);

	glPopAttrib();
	glPopMatrix();
}

void drawObstacles(){
	glPushMatrix();
	//Objects
	for(int j=0; j < OBJECT_COUNT; j++) {
		Obstacle *obs;
		obs = obstaclesList[j];
		
		if (obs->z > eye[2]) {
			if(! --obs->obj->timesUsed)
				obs->obj->unload();
			
			obs->z -= obstaclegap * OBJECT_COUNT;
			obs->x = (randf() - 0.5) * 1.5 * maxX;
			obs->y = (randf() - 0.5) * 1.5 * maxY;
			obs->obj = &( allObjects[rand() % allObjects.size()] );
			obs->obj->timesUsed++;
			obs->obj->load();
			
			tosave ++;
			tosave %= OBJECT_COUNT;
		}
		else {
			if (!drawless && tosave == j)
				obs->obj->save = true;
			
			drawObstacle(obs);
			
			if (!drawless && obs->obj->save)	{
				collide = collide || plane.collision(obs->obj);
				delete[] obs->obj->modelView;
				obs->obj->modelView = NULL;
			}
			
			obs->obj->save = false;
		}
	}
	
	glPopMatrix();
}

// glut's Main Display Function
void drawScene()	{
	glPushMatrix();

	if(!drawless)
		drawTerrain();

	drawObstacles();
	
	glPopMatrix();
}

void display()	{
	
	//Calculate & save matrices
	glPushMatrix();
	
	glLoadIdentity();
	gluPerspective(zoom * 65.0f, 1.0f, 0.5f, 2000.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(cameraProj));
	
	glLoadIdentity();
    if (ended)
        gluLookAt(eye[0],shake(), eye[2], center[0], center[1], center[2], up[0], up[1], up[2]);
    else
        gluLookAt(eye.x, eye.y, eye.z,
			  center.x, center.y, center.z,
			  up.x, up.y, up.z);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(cameraView));
	
	glLoadIdentity();
	glOrtho(-100.0f, 100.0f, -35.0f, 150.0f, 75.0f, 1000.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(lightProj));
	
	glLoadIdentity();
	gluLookAt(lightpos.x, lightpos.y, lightpos.z,
			  lightpos.x - lightdir.x, lightpos.y - lightdir.y, lightpos.z - lightdir.z,
			  0.0f, 1.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, glm::value_ptr(lightView));
	
	glPopMatrix();
	
	// reconstruct buffers only if necessary
	if(shadowW != winW * SHADOW_MAP_RATIO || shadowH != winH * SHADOW_MAP_RATIO)	{
		glDeleteTextures(1, &shadowmap);
		glDeleteFramebuffers(1, &shadowFBO);

		shadowW = winW * SHADOW_MAP_RATIO;
		shadowH = winH * SHADOW_MAP_RATIO;
		genDepthFBO(&shadowFBO, &shadowmap, shadowW, shadowH);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	}
	
	// Begin actual stuff
	glClearColor(0.0, 0.0, 0.0, 0.0);
	setFrameBuffer(shadowFBO);
	
	drawless = true;
	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
	glDisable(GL_BLEND);
	glShadeModel(GL_FLAT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	//First pass - from light's point of view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(lightProj));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Don't use lightView, to avoid shadow acne
	//	glLoadMatrixf(glm::value_ptr(lightView));
	// Instead, let's move the light a little bit towards the center.
	glm::vec3 _pos = lightpos + (2 * 1e-3f * lightdir);
	gluLookAt(_pos.x, _pos.y, _pos.z,
			  lightpos.x - lightdir.x, lightpos.y - lightdir.y, lightpos.z - lightdir.z,
			  0.0f, 1.0f, 0.0f);

	//Use viewport the same size as the shadow map
	glViewport(0, 0, shadowW, shadowH);

	glTranslatef(0.0f, 0.0f, curZ);
	drawPlane();
	glTranslatef(0.0f, 0.0f, -curZ);
	drawScene();

	//2nd pass - Draw from camera's point of view
	setFrameBuffer(0);
	drawless = false;
	glEnable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glEnable(GL_FOG);
	glShadeModel(GL_SMOOTH);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(cameraProj));
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(cameraView));

	glViewport(wLo, hLo, wHi - wLo, hHi - hLo);

	//Use dim light to represent shadowed areas
	glLightfv(GL_LIGHT0, GL_POSITION, glm::value_ptr(glm::vec4(lightdir, 0.0)));
	glLightfv(GL_LIGHT0, GL_AMBIENT, glm::value_ptr(glm::vec4(0.2, 0.2, 0.2, 0.0)));
	glLightfv(GL_LIGHT0, GL_DIFFUSE, glm::value_ptr(glm::vec4(0.2, 0.2, 0.2, 1.0)));
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(glm::vec4(0, 0, 0, 0)));

	glTranslatef(0.0f, 0.0f, curZ);
	RenderSkybox(glm::vec3(0.0f, -150.0f, 0.0f), environment);
	glTranslatef(0.0f, 0.0f, -curZ);

	glTranslatef(0.0f, 0.0f, curZ);
	drawPlane();
	glTranslatef(0.0f, 0.0f, -curZ);
	drawScene();

	//3rd pass
	//Draw with bright light
	glLightfv(GL_LIGHT0, GL_SPECULAR, glm::value_ptr(glm::vec4(specular, 1.0f)));
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  glm::value_ptr(glm::vec4(diffuse, 1.0f)));
	glLightfv(GL_LIGHT0, GL_AMBIENT,  glm::value_ptr(glm::vec4(ambient, 1.0f)));

	//Calculate texture matrix for projection
	//This matrix takes us from eye space to the light's clip space
	//It is postmultiplied by the inverse of the current view matrix when specifying texgen
	glm::mat4 biasMatrix(	0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, 0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.5f, 0.0f,
							0.5f, 0.5f, 0.5f, 1.0f	);	//bias from [-1, 1] to [0, 1]
	glm::mat4 textureMatrix = biasMatrix * lightProj * lightView;
	glm::mat4 texTrans = glm::transpose(textureMatrix);

	glTranslatef(0.0f, 0.0f, curZ);
	drawPlane();
	glTranslatef(0.0f, 0.0f, -curZ);

	glActiveTexture(GL_TEXTURE5);
		//Bind & enable shadow map texture
		glBindTexture(GL_TEXTURE_2D, shadowmap);
		glEnable(GL_TEXTURE_2D);
		
		//Set up texture coordinate generation.
		glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_S, GL_EYE_PLANE, glm::value_ptr(texTrans[0]));
		glEnable(GL_TEXTURE_GEN_S);

		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_T, GL_EYE_PLANE, glm::value_ptr(texTrans[1]));
		glEnable(GL_TEXTURE_GEN_T);

		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_R, GL_EYE_PLANE, glm::value_ptr(texTrans[2]));
		glEnable(GL_TEXTURE_GEN_R);

		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_Q, GL_EYE_PLANE, glm::value_ptr(texTrans[3]));
		glEnable(GL_TEXTURE_GEN_Q);

		//Enable shadow comparison
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);

		//Shadow comparison should generate an INTENSITY result
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);
		
		//Shadow comparison should be true (ie not in shadow) if r<=texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);

	//Set alpha test to discard false comparisons
	glAlphaFunc(GL_GEQUAL, 0.99f);
	glEnable(GL_ALPHA_TEST);

	drawScene();

	//Disable textures and texgen
	glActiveTexture(GL_TEXTURE5);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
	glActiveTexture(GL_TEXTURE0);

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	exp();
	drawStars();

	if(boost)	{
		glAccum(GL_MULT, 0.5);
		glAccum(GL_ACCUM, 0.5);
		glAccum(GL_RETURN, 1.0);
		collide = ended = 0;
	}
	else if(collide && !ended)	{
		ended = 1;
		cout << "Collision!\n";
		openal.play(2);
		pause();
		run = 1;
		collisionFrame = frameCount;
		step = 0;
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

void iter(int single)	{
	if(run)	{
		
		curZ -= boost ? step * 2 : step;
		lightpos.z -= boost ? step * 2 : step;
		
		glutPostRedisplay();
		
	} else if(single)	{
		
		curZ -= step;
		lightpos.z = curZ + 6;
		glutPostRedisplay();
		
	}
	
	eye[2] = curZ + 10.0f;
	center[2] = curZ - 10.0f;
	
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
	openal.init();

	// Need to double buffer for animation
	glutInit(&argc,argv);
	glutInitDisplayMode( GLUT_ACCUM | GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	
	// Create and position the graphics window
	glutInitWindowPosition( 100, 100 );
	winW = winH = 500;
	glutInitWindowSize( winW, winH );
	glutCreateWindow( "PaperPlane 3D" );
	
	//initialize constants
	preGLInit();
	// Initialize OpenGL.
	GLInit();
	//initialize things which need GLInit before
	postGLInit();
	
	// Callback for graphics image redrawing
	glutDisplayFunc( display );
	
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
	openal.play(0);
	glutMainLoop(  );
    
	return 0;
}