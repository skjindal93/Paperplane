//
//  image.cpp
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

Image* ReadBMP(const char* filename)
{
    FILE* f = fopen(filename, "rb");
	
    if(f == NULL)
        throw "Argument Exception";
	
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
	
    // extract image height and width from header
    int w = *(int*)&info[18];
    int h = *(int*)&info[22];
	
    cout << endl;
	
    int row_padded = (w*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;
	
    for(int i = 0; i < h; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < w*3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j+2];
            data[j+2] = tmp;
			
            cout << "R: "<< (int)data[j] << " G: " << (int)data[j+1]<< " B: " << (int)data[j+2]<< endl;
        }
    }
	
    fclose(f);
	
	Image *obj;
	obj = new Image(w, h);
	
    for(int i = 0; i < w; ++i)
    	for(int j = 0; j < h; ++j)	{
    		obj->pixel[i][j][0] = (double)data[3 * (i + w * j) + 0] / 255.0;
    		obj->pixel[i][j][1] = (double)data[3 * (i + w * j) + 1] / 255.0;
    		obj->pixel[i][j][2] = (double)data[3 * (i + w * j) + 2] / 255.0;
    	}
	
	delete[] data;

    return obj;
}

GLuint loadTexture(const char* file)	{
    FILE* f = fopen(file, "rb");
	
    if(f == NULL)
        throw "Argument Exception";
	
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header
	
    // extract image height and width from header
    int w = *(int*)&info[18];
    int h = *(int*)&info[22];
	
    cout << endl;
	
    int row_padded = (w*3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    unsigned char tmp;
	
    for(int i = 0; i < h; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for(int j = 0; j < w*3; j += 3)
        {
            // Convert (B, G, R) to (R, G, B)
            tmp = data[j];
            data[j] = data[j+2];
            data[j+2] = tmp;
			
            cout << "R: "<< (int)data[j] << " G: " << (int)data[j+1]<< " B: " << (int)data[j+2]<< endl;
        }
    }
	
    fclose(f);
	
	GLuint texture;
	glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	delete[] data;
	
	return texture;
}