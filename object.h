//
//  object.h
//  PaperPlane
//
//  Created by Shivanker Goel on 31/10/13.
//  Copyright (c) 2013 Shivanker. All rights reserved.
//

#ifndef __PaperPlane__object__
#define __PaperPlane__object__

#include "paperplane.h"

class Material	{
public:
	GLfloat s;
	glm::vec3 Ka, Kd, Ks, Ke;
	char* name;
	
	Material();
	
	void enable();
	void apply();
	void disable();
	
	~Material();
};

vector<Material>* readMTL(const char* file);

struct triangle	{
	glm::ivec3 v, t, n;
};

class Object	{
public:
	bool loaded, useTex, useNormal;
	GLuint texture, vertexVBO, textureVBO, normalVBO;
	vector<glm::vec3> normals, vertices;
	vector<glm::vec2> uvs;
	vector<triangle> triangles;
	Material *mtl;
	char* name;
	
	Object();
	
	void load();
	void render(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);
	void render(GLfloat scale = 1.0f);
	void unload();
	
	~Object();

};

vector<Object>* readOBJ(const char* file);

#endif /* defined(__PaperPlane__object__) */
