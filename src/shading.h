//
//  shading.h
//  PaperPlane
//
//  Created by Shivanker Goel and Shubham Jindal on 09/11/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__shading__
#define __PaperPlane__shading__

#include "paperplane.h"
#define BUFFER_SIZE 2048

class Shader	{
public:
	unsigned int pID, vID, fID;
	
	Shader();
	Shader(string vertFile, string fragFile);

	void init(char* vert, char* frag);
	void initFromFile(string vertFile, string fragFile);
	void bind();
	void unbind();
	
	~Shader();
};

#endif /* defined(__PaperPlane__shading__) */
