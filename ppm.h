//
//  ppm.h
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__ppm__
#define __PaperPlane__ppm__

#include "paperplane.h"

class Image	{
public:
	int w, h;
	glm::vec3 **pixel;

	Image(int w, int h);
	~Image();
};

Image* readP6(const char* file);
GLuint loadTexture(const char* file);

#endif /* defined(__PaperPlane__ppm__) */