#include "paperplane.h"

#define NUM_PARTICLES    1000          /* Number of particles  */
#define NUM_DEBRIS       50            /* Number of debris     */

struct particleData
{
    float   position[3];
    float   speed[3];
    float   color[3];
};
typedef struct particleData particleData;


/* A piece of debris */
struct debrisData
{
    float   position[3];
    float   speed[3];
    float   orientation[3];        /* Rotation angles around x, y, and z axes */
    float   orientationSpeed[3];
    float   color[3];
    float   scale[3];
};
typedef struct debrisData debrisData;


/* Globals */
particleData     particles[NUM_PARTICLES];
debrisData       debris[NUM_DEBRIS];
int              fuel = 0;                /* "fuel" of the explosion */

// Randomize a new speed vector.
void newSpeed (float dest[3])
{
    float    x;
    float    y;
    float    z;
    float    len;
    
    x = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
    y = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
    z = (2.0 * ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;

    
    len = 0.25 * sqrt (x * x + y * y + z * z);
    
    if (len)
	{
        x = x / len;
        y = y / len;
        z = z / len;
	}
    
    
    dest[0] = x;
    dest[1] = y;
    dest[2] = z - 3.0;
}


// Create a new explosion.
void newExplosion (int x, int y, int z)
{
    int i;
    
    for (i = 0; i < NUM_PARTICLES; i++)
    {
        particles[i].position[0] = x;
        particles[i].position[1] = y;
        particles[i].position[2] = z;
        
        particles[i].color[0] = 1.0;
        particles[i].color[1] = 1.0;
        particles[i].color[2] = 0.5;
        
        newSpeed (particles[i].speed);
    }
    
    for (i = 0; i < NUM_DEBRIS; i++)
    {
        debris[i].position[0] = x;
        debris[i].position[1] = y;
        debris[i].position[2] = z;
        
        debris[i].orientation[0] = 0.0;
        debris[i].orientation[1] = 0.0;
        debris[i].orientation[2] = 0.0;
        
        debris[i].color[0] = 0.7;
        debris[i].color[1] = 0.7;
        debris[i].color[2] = 0.7;
        
        debris[i].scale[0] = (2.0 *
                              ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
        debris[i].scale[1] = (2.0 *
                              ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
        debris[i].scale[2] = (2.0 *
                              ((GLfloat) rand ()) / ((GLfloat) RAND_MAX)) - 1.0;
        
        newSpeed (debris[i].speed);
        newSpeed (debris[i].orientationSpeed);
    }
    
    fuel = 1000;
}

void exp(void){
    int i;

    if (fuel > 0)
    {
        glPushAttrib(GL_ENABLE_BIT);
		glPushAttrib(GL_CURRENT_BIT);
        glPushMatrix ();
        glDisable (GL_LIGHTING);
        glDisable (GL_DEPTH_TEST);
        
        glBegin (GL_POINTS);
        
        for (i = 0; i < NUM_PARTICLES; i++)
        {
            glColor3fv (particles[i].color);
            glVertex3fv (particles[i].position);
        }
        
        glEnd ();
        
        glPopMatrix ();
        glNormal3f (0.0, 0.0, 1.0);
        

        for (i = 0; i < NUM_DEBRIS; i++)
        {
            glColor3fv (debris[i].color);
            
            glPushMatrix ();
            
            glTranslatef (debris[i].position[0],
                          debris[i].position[1],
                          debris[i].position[2]);
            
            glRotatef (debris[i].orientation[0], 1.0, 0.0, 0.0);
            glRotatef (debris[i].orientation[1], 0.0, 1.0, 0.0);
            glRotatef (debris[i].orientation[2], 0.0, 0.0, 1.0);
            
            glScalef (debris[i].scale[0],
                      debris[i].scale[1],
                      debris[i].scale[2]);
            
            glBegin (GL_TRIANGLES);
            glVertex3f (0.0, 0.5, 0.0);
            glVertex3f (-0.25, 0.0, 0.0);
            glVertex3f (0.25, 0.0, 0.0);
            glEnd ();
            
            glPopMatrix ();
        }
		glPopAttrib();
		glPopAttrib();

		for (i = 0; i < NUM_PARTICLES; i++)
	    {
            particles[i].position[0] += particles[i].speed[0] * 0.2;
            particles[i].position[1] += particles[i].speed[1] * 0.2;
            particles[i].position[2] += particles[i].speed[2] * 0.2;

            particles[i].color[0] -= 1.0 / 500.0;
            if (particles[i].color[0] < 0.0)
            {
                particles[i].color[0] = 0.0;
            }

            particles[i].color[1] -= 1.0 / 100.0;
            if (particles[i].color[1] < 0.0)
            {
                particles[i].color[1] = 0.0;
            }

            particles[i].color[2] -= 1.0 / 50.0;
            if (particles[i].color[2] < 0.0)
            {
                particles[i].color[2] = 0.0;
            }
	    }
        for (i = 0; i < NUM_DEBRIS; i++)
	    {
            debris[i].position[0] += debris[i].speed[0] * 0.1;
            debris[i].position[1] += debris[i].speed[1] * 0.1;
            debris[i].position[2] += debris[i].speed[2] * 0.1;

            debris[i].orientation[0] += debris[i].orientationSpeed[0] * 10;
            debris[i].orientation[1] += debris[i].orientationSpeed[1] * 10;
            debris[i].orientation[2] += debris[i].orientationSpeed[2] * 10;
	    }

        --fuel;
	}
}
