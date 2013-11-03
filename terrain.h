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
	int w, h, nvert, ntri, lod;
	GLuint texture, vertexVBO, textureVBO, normalVBO;
	float** heights;
	glm::vec3** normals;

	Terrain(Image* hmap, const char* texture = NULL, int lod = 1);
	void computeNormals();
	void render(GLfloat height, GLfloat size, GLfloat starting = 0, GLfloat fraction = 1.0f);
	void buildArrayBuffers();
	void setLOD(int LOD);
	
	~Terrain();
};


#endif /* defined(__PaperPlane__terrain__) */
