//
//  terrain.h
//  PaperPlane
//
//  Created by Shivanker Goel on 19/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__terrain__
#define __PaperPlane__terrain__

#include "paperplane.h"

class Terrain {
public:
	int w, h, nvert, ntri;
	GLuint texture, vertexVBO, textureVBO, normalVBO;
	float** heights;
	glm::vec3** normals;

	Terrain(Image* hmap, const char* texture = NULL);
	void computeNormals();
	void render(GLfloat height, GLfloat size);
	void buildArrayBuffers();
	
	~Terrain();
};


#endif /* defined(__PaperPlane__terrain__) */
