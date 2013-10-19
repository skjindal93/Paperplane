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

#endif /* defined(__PaperPlane__ppm__) */


class Image	{
public:
	int w, h;
	float ***pixel;
	Image(int w, int h);
	~Image();
};

Image* readP6(char file[]);