//
//  al.h
//  PaperPlane
//
//  Created by Shivanker Goel and Shubham Jindal on 29/11/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__al__
#define __PaperPlane__al__

#include "paperplane.h"

#define NUM_SOURCES 3
#define NUM_BUFFERS NUM_SOURCES
#define NUM_ENVIRONMENTS 1

class al {
public:
	ALfloat listenerPos[3] = {0.0,0.0,4.0};
	ALfloat listenerVel[3] = {0.0,0.0,0.0};
	ALfloat listenerOri[6] = {0.0,0.0,1.0, 0.0,1.0,0.0};
	ALuint  buffer[NUM_BUFFERS];
	ALuint  source[NUM_SOURCES];
	ALuint  environment[NUM_ENVIRONMENTS];

	ALsizei size,freq;
	ALenum  format;
	ALvoid  *data;

	void init();
	void play(int i);
	void stop(int i);

};

#endif /* defined(__PaperPlane__al__) */
