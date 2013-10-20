//
//  ppm.h
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__ppm__
#define __PaperPlane__ppm__

#include <iostream>
using namespace std;

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Image	{
public:
	int w, h;
	glm::vec3 **pixel;

	Image(int w, int h);
	~Image();
};

Image* readP6(char* file);

#endif /* defined(__PaperPlane__ppm__) */