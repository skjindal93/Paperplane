//
//  image.h
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__image__
#define __PaperPlane__image__

#include "paperplane.h"

class Image	{
public:
	int w, h;
	glm::vec3 **pixel;

	Image(int w, int h);
	~Image();
};

Image* readP3(string file);
GLuint loadTexture(string file);

#endif /* defined(__PaperPlane__image__) */