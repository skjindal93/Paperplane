//
//  ppm.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "paperplane.h"

Image::Image(int w, int h)	{
	this->w = w;
	this->h = h;
	pixel = new glm::vec3*[w];
	for (int i = 0; i < w; ++i)
		pixel[i] = new glm::vec3[h];
}

Image::~Image()	{
	for (int i = 0; i < w; ++i)
		delete[] pixel[i];
	delete[] pixel;
}

Image* readP3(const char* file)	{
	ifstream img;
	img.open(file);
	char buff[16];
	
	img >> buff;
	//check the image format
    if (buff[0] != 'P' || buff[1] != '3') {
		cerr << "File not in the standard P3 format." << endl;
		return NULL;
	}
	
	int w, h, alpha;
	//read image size information
	img >> w >> h >> alpha;
	
    //read rgb component
    if (alpha > 255 || alpha < 0 || w < 0 || h < 0) {
        cerr << "Invalid size/alpha specifications for image.";
        return NULL;
    }
	
	Image *obj;
	obj = new Image(w, h);
	
	//read pixel data from file
	for(int i = 0; i < h; ++i)
		for(int j = 0; j < w; ++j)	{
			img >> obj->pixel[j][i][0] >> obj->pixel[j][i][1] >> obj->pixel[j][i][2];
			obj->pixel[j][i][0] /= (double)alpha;
			obj->pixel[j][i][1] /= (double)alpha;
			obj->pixel[j][i][2] /= (double)alpha;
		}
	img.close();
	
	return obj;
}

GLuint loadTexture(const char* file)	{
	ifstream img;
	img.open(file);
	char buff[16];
	
	img >> buff;
	//check the image format
    if (buff[0] != 'P' || buff[1] != '3') {
		cerr << "Texture file not in the standard P3 format." << endl;
		return NULL;
	}
	
	int w, h, alpha;
	//read image size information
	img >> w >> h >> alpha;
	
    //read rgb component
    if (alpha != 255 || w < 0 || h < 0) {
        cerr << "Invalid size/alpha specifications for texture.";
        return NULL;
    }
	
	unsigned char data[w * h * 3 + 2];
	
	//read pixel data from file
	for(int i = 0; i < h; ++i)
		for(int j = 0; j < w; ++j)	{
			int temp;
			img >> temp;
			data[(i * w + j)*3 + 0] = temp;
			img >> temp;
			data[(i * w + j)*3 + 1] = temp;
			img >> temp;
			data[(i * w + j)*3 + 2] = temp;
		}
	img.close();

	GLuint texture;
	glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
	return texture;
}