//
//  al.cpp
//  PaperPlane
//
//  Created by Shivanker Goel and Shubham Jindal on 29/11/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"

void al::init()	{
	alutInit(0, NULL);

	alListenerfv(AL_POSITION,listenerPos);
	alListenerfv(AL_VELOCITY,listenerVel);
	alListenerfv(AL_ORIENTATION,listenerOri);

	alGetError(); // clear any error messages

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);

	if(alGetError() != AL_NO_ERROR)
	{
		printf("- Error creating AL buffers !!\n");
		exit(1);
	}

	char music[200];
	strcpy(music, (string(PATH) + "h2h.wav").c_str());

	alutLoadWAVFile(music, &format, &data, &size, &freq);
	alBufferData(buffer[0],format,data,size,freq);
	alutUnloadWAV(format, data, size, freq);

	strcpy(music, (string(PATH) + "star.wav").c_str());
	alutLoadWAVFile(music, &format, &data, &size, &freq);
	alBufferData(buffer[1],format,data,size,freq);
	alutUnloadWAV(format, data, size, freq);

	strcpy(music, (string(PATH) + "end.wav").c_str());
	alutLoadWAVFile(music, &format, &data, &size, &freq);
	alBufferData(buffer[2],format,data,size,freq);
	alutUnloadWAV(format, data, size, freq);

	if(alGetError() != AL_NO_ERROR)
	{
		printf("- Error loading wav !!\n");
		exit(2);
	}

	alGenSources(NUM_SOURCES, source);

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 0.5f);
	alSourcefv(source[0], AL_POSITION, listenerPos);
	alSourcefv(source[0], AL_VELOCITY, listenerVel);
	alSourcei(source[0], AL_BUFFER,buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 1.0f);
	alSourcefv(source[1], AL_POSITION, listenerPos);
	alSourcefv(source[1], AL_VELOCITY, listenerVel);
	alSourcei(source[1], AL_BUFFER,buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_FALSE);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 1.0f);
	alSourcefv(source[2], AL_POSITION, listenerPos);
	alSourcefv(source[2], AL_VELOCITY, listenerVel);
	alSourcei(source[2], AL_BUFFER,buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_FALSE);

	if(alGetError() != AL_NO_ERROR)
	{
		printf("- Error creating sources !!\n");
		exit(2);
	}
}

void al::play(int i)	{
	alSourcePlay(source[i]);
}

void al::stop(int i)	{
	alSourceStop(source[i]);
}