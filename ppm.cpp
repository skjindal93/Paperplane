//
//  ppm.cpp
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#include "ppm.h"

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


Image* readP6(char file[])	{
	FILE *img;
	img = fopen(file, "rb");
	char buff[16];
	
	fgets(buff, sizeof(buff), img);
	//check the image format
    if (buff[0] != 'P' || buff[1] != '6') {
		cerr << "File not in the standard P6 format." << endl;
		return NULL;
	}
	
	//check for comments
    int c = getc(img);
    while (c == '#') {
		#pragma GCC diagnostic ignored "-Wempty-body"
    	while (getc(img) != '\n');
        c = getc(img);
    }
	ungetc(c, img);
	
	int w, h;
	//read image size information
    if (fscanf(img, "%d %d", &w, &h) != 2) {
        cerr << "Invalid size for image." << endl;
        return NULL;
    }
	
	int alpha = 0;
	
    //read rgb component
    if (fscanf(img, "%d", &alpha) != 1 || alpha > 255 || alpha < 0) {
        cerr << "Invalid RGB component for image. Expected an integer in the range 0 to 255.";
        return NULL;
    }
    while (fgetc(img) != '\n') ;
	
	Image *obj;
	obj = new Image(w, h);
	
	//read pixel data from file
	unsigned char *line;
	line = new unsigned char[w * 3 * h + 5];
    if (fread(line, 3 * w, h, img) != h)
        cerr << "Could not read the whole image file! Trying to proceed.";
    fclose(img);
	
    for(int i = 0; i < w; ++i)
    	for(int j = 0; j < h; ++j)	{
    		obj->pixel[i][j][0] = (double)line[3 * (i + w * j) + 0] / (double)alpha;
    		obj->pixel[i][j][1] = (double)line[3 * (i + w * j) + 1] / (double)alpha;
    		obj->pixel[i][j][2] = (double)line[3 * (i + w * j) + 2] / (double)alpha;
    	}
    delete[] line;
	
	return obj;
}