//
//  shading.h
//  PaperPlane
//
//  Created by Shivanker Goel on 09/11/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__shading__
#define __PaperPlane__shading__

#include "paperplane.h"
#define BUFFER_SIZE 2048
#define SHADER_VERT 1
#define SHADER_FRAG 2
#define SHADER_BOTH SHADER_VERT|SHADER_FRAG
#define SHADER_NONE 0

class Shader	{
public:
	unsigned int pID, vID, fID, activate;
	
	Shader(int activate = SHADER_BOTH);
	Shader(string vertFile, string fragFile, int activate = SHADER_BOTH);
	
	void init(string vertFile, string fragFile);
	void bind();
	void unbind();
	
	~Shader();
};

#endif /* defined(__PaperPlane__shading__) */
